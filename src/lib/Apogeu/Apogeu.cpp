#include "Apogeu.h"

///Rotinas de verificacao de apogeu

#ifdef ARDUINO_AVR_MEGA2560
Apogeu::Apogeu(unsigned int n, unsigned int r, float s) : N(n), R((r > 1) ? r : 2), Rl1((r > 1) ? r - 1 : 1), S(s), Height(N, 5), Rf(Rl1 * (Rl1 + 1) * (2 * Rl1 + 1) / 6)
#else
Apogeu::Apogeu(unsigned int n, unsigned int r, float s) : N(n), R((r > 1) ? r : 2), Rl1((r > 1) ? r - 1 : 1), S(s), Height(N), Rf(Rl1 * (Rl1 + 1) * (2 * Rl1 + 1) / 6)
#endif // ARDUINO_AVR_MEGA2560
{
	//for (int i = Rl1; i > 0; i--) Rf += (float)(i*i);
}

float Apogeu::addZero(long P, float sealevelP)
{
	// float currentHeight = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577);
	float currentHeight = 44330.0 * (1 - pow(float(P) / sealevelP, 1 / 5.25588));
	if (baseIndex == 0)
	{
		base = baseMax = baseMin = currentHeight;
	}
	else
	{
		base = ((base*baseIndex) + currentHeight) / (baseIndex + 1);
		if(currentHeight > baseMax) baseMax = currentHeight;
		if(currentHeight < baseMin) baseMin = currentHeight;
	}
	baseIndex++;
	return base;
}

float Apogeu::getZero()
{
	return base;
}

bool Apogeu::fixZero(float maxRange)
{
	float eeBase;
	EEPROM.get(eeAddress, eeBase);

	bool eeValid = true;
	eeValid &= !isnan(eeBase);
	eeValid &= !isinf(eeBase);
	eeValid &= eeBase <= 44330.0f; // Max value of the barometric equation
	eeValid &= eeBase >= -44330.0f; // Arbitrary value

	if(eeValid && (baseMax - baseMin >= maxRange)) {
		base = eeBase;
		usingFixZero = true;
	} else if(!eeValid || (abs(eeBase - base) >= maxRange)){
		EEPROM.put(eeAddress, base);
	}

	return usingFixZero;
}
bool Apogeu::getFixZero(){
	return usingFixZero;
}

uint16_t Apogeu::getEEAddress(){
	return eeAddress;
}

void Apogeu::resetZero()
{
	base = 0;
	baseIndex = 0;
}

void Apogeu::resetAptPt()
{
	apgPt = heightMed[0];
}

void Apogeu::resetTimer()
{
	TimeZero = micros();
}

//float Apogeu::addAltitude(float H)
//{
//	for (int i = N - 1; i > 0; i--)
//	{
//		Height[i] = Height[i - 1];
//	}
//	Height[0] = H;
//	float Sum = 0;
//	for (uint8_t i = 0; i < N; i++)
//	{
//		Sum += Height[i];
//	}
//	for (uint8_t i = Rl1; i > 0; i--)
//	{
//		heightMed[i] = heightMed[i - 1];
//	}
//	heightMed[0] = Sum / N;
//	if (heightMed[0] > apgPt)
//	{
//		apgPt = heightMed[0];
//		apgTm = micros() - TimeZero;
//	}
//	minH = (heightMed[0] < minH) ? heightMed[0] : minH;
//	maxH = (heightMed[0] > maxH) ? heightMed[0] : maxH;
//	return heightMed[0];
//}

float Apogeu::calcHeight(const long &P, float sealevelP)
{
	//for (int i = N - 1; i > 0; i--)
	//{
	//	Height[i] = Height[i - 1];
	//}
	//Height[0] = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577) - base;
	//float Sum = 0;
	//for (uint8_t i = 0; i < N; i++)
	//{
	//	Sum += Height[i];
	//}
	for (uint8_t i = Rl1; i > 0; i--)
	{
		heightMed[i] = heightMed[i - 1];
	}
	//heightMed[0] = Sum / N;
	heightMed[0] = Height = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577) - base;
	if (heightMed[0] > apgPt)
	{
		apgPt = heightMed[0];
		apgTm = micros() - TimeZero;
	}
	minH = (heightMed[0] < minH) ? heightMed[0] : minH;
	maxH = (heightMed[0] > maxH) ? heightMed[0] : maxH;
	return heightMed[0];
}

void Apogeu::setGamma(bool apgE)
{
	Gamma = apgE;
}

bool Apogeu::apgAlpha(bool serial)
{
	Alpha = 1;
	for (byte i = Rl1; i > 0; i--)
	{
		if (heightMed[i] > heightMed[i - 1]) Alpha &= 1;
		else Alpha &= 0;
		if (!Alpha) break;
		//if (serial) Serial.print(heightMed[i] > heightMed[i - 1]);
	}
	return Alpha;
}

bool Apogeu::apgBeta()
{
	return Beta = heightMed[0] < apgPt - S;
}

bool Apogeu::apgGamma()
{
	return Gamma;
}

float Apogeu::apgSigma(bool serial)
{
	Sigma = 0;
	for (unsigned int i = 0; i < Rl1; i++) cond[i] = 1;
	for (int i = Rl1; i > 0; i--)
	{
		for (int j = Rl1; j > 0; j -= (R - i))
		{
			int all = j - (R - i);
			if (all < 0) continue;
			cond[i - 1] &= (heightMed[j] > heightMed[all]);
			if (!cond[i - 1]) break;
		}
	}
	//if (serial) for (int i = Rl1 - 1; i >= 0; i--) Serial.print(cond[i]);
	for (unsigned int i = Rl1; i > 0; i--)
	{
		Sigma += (float)cond[i - 1] * (float)(i*i) / Rf;
	}
	return Sigma;
}

bool Apogeu::getApogeu(float percent, bool type)
{
	if (type) return Alpha || Beta || Gamma || (Sigma > percent);
	else
	{
		if (!apg_H && (Alpha || Beta || Gamma || (Sigma > percent)))
		{
			apg_H = 1;
			return 1;
		}
		else return 0;
	}
}

bool Apogeu::getAlpha()
{
	return Alpha;
}

bool Apogeu::getBeta()
{
	return Beta;
}

bool Apogeu::getGamma()
{
	return Gamma;
}

float Apogeu::getSigma()
{
	return Sigma;
}

float Apogeu::getHeight()
{
	return heightMed[0];
}

float Apogeu::getHeight(float B)
{
	return heightMed[0] + base - B;
}

float Apogeu::getApgPt()
{
	return (float)apgPt;
}

float Apogeu::getApgTm()
{
	return (float)apgTm / 1000000.0;
}

float Apogeu::getMaxH()
{
	return maxH;
}

float Apogeu::getMinH()
{
	return minH;
}