#include "Apogeu.h"

///Rotinas de verificacao de apogeu

#ifdef ARDUINO_AVR_MEGA2560
Apogeu::Apogeu(unsigned int n, unsigned int r, float s) : N(n), R((r > 1) ? r : 2), Rl1((r > 1) ? r - 1 : 1), S(s), Alt(N, 5), Rf(Rl1 * (Rl1 + 1) * (2 * Rl1 + 1) / 6)
#else
Apogeu::Apogeu(unsigned int n, unsigned int r, float s) : N(n), R((r > 1) ? r : 2), Rl1((r > 1) ? r - 1 : 1), S(s), Alt(N), Rf(Rl1 * (Rl1 + 1) * (2 * Rl1 + 1) / 6)
#endif // ARDUINO_AVR_MEGA2560
{
	//for (int i = Rl1; i > 0; i--) Rf += (float)(i*i);
}

float Apogeu::addZero(long P, float sealevelP)
{
	if (baseIndex == 0)
	{
		base = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577);
	}
	else
	{
		base = ((base*baseIndex) + ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577)) / (baseIndex + 1);
	}
	baseIndex++;
	return base;
}

float Apogeu::getZero()
{
	return base;
}

void Apogeu::resetZero()
{
	base = 0;
	baseIndex = 0;
}

void Apogeu::resetTimer()
{
	TimeZero = micros();
}

//float Apogeu::addAltitude(float H)
//{
//	for (int i = N - 1; i > 0; i--)
//	{
//		Alt[i] = Alt[i - 1];
//	}
//	Alt[0] = H;
//	float Sum = 0;
//	for (uint8_t i = 0; i < N; i++)
//	{
//		Sum += Alt[i];
//	}
//	for (uint8_t i = Rl1; i > 0; i--)
//	{
//		altMed[i] = altMed[i - 1];
//	}
//	altMed[0] = Sum / N;
//	if (altMed[0] > apgPt)
//	{
//		apgPt = altMed[0];
//		apgTm = micros() - TimeZero;
//	}
//	minH = (altMed[0] < minH) ? altMed[0] : minH;
//	maxH = (altMed[0] > maxH) ? altMed[0] : maxH;
//	return altMed[0];
//}

float Apogeu::calcAlt(const long &P, float sealevelP)
{
	//for (int i = N - 1; i > 0; i--)
	//{
	//	Alt[i] = Alt[i - 1];
	//}
	//Alt[0] = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577) - base;
	//float Sum = 0;
	//for (uint8_t i = 0; i < N; i++)
	//{
	//	Sum += Alt[i];
	//}
	for (uint8_t i = Rl1; i > 0; i--)
	{
		altMed[i] = altMed[i - 1];
	}
	//altMed[0] = Sum / N;
	altMed[0] = Alt = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577) - base;
	if (altMed[0] > apgPt)
	{
		apgPt = altMed[0];
		apgTm = micros() - TimeZero;
	}
	minH = (altMed[0] < minH) ? altMed[0] : minH;
	maxH = (altMed[0] > maxH) ? altMed[0] : maxH;
	return altMed[0];
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
		if (altMed[i] > altMed[i - 1]) Alpha &= 1;
		else Alpha &= 0;
		if (!Alpha) break;
		//if (serial) Serial.print(altMed[i] > altMed[i - 1]);
	}
	return Alpha;
}

bool Apogeu::apgBeta()
{
	return Beta = altMed[0] < apgPt - S;
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
			cond[i - 1] &= (altMed[j] > altMed[all]);
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
		if (!apg_A && (Alpha || Beta || Gamma || (Sigma > percent)))
		{
			apg_A = 1;
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

float Apogeu::getAltitude()
{
	return altMed[0];
}

float Apogeu::getAltitude(float B)
{
	return altMed[0] + base - B;
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