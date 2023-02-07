#include "Classes.h"

/*
///Filtro de media movel
MediaMovel::MediaMovel(int n) : N(n)
{
	for (unsigned int i = 0; i < N; i++) Vals[i] = 0;
}
MediaMovel::~MediaMovel()
{
	delete[] Vals;
}
float MediaMovel::addValor(const float &valor)
{
	for (unsigned int i = N - 1; i > 0; i--)
	{
		Vals[i] = Vals[i - 1];
	}
	Vals[0] = valor;
	media = 0;
	for (unsigned int i = 0; i < N; i++)
	{
		media += Vals[i];
	}
	media /= N;
	max = (media > max) ? media : max;
	min = (media < min) ? media : min;
	return media;
}
float MediaMovel::getMedia()
{
	return media;
}
float MediaMovel::getVar()
{
	sigma = 0;
	for (unsigned int i = 0; i < N; i++)
	{
		sigma += pow(Vals[i] - media, 2);
	}
	sigma = pow(sigma / (N - 1), .5);
	return sigma;
}
float MediaMovel::getMax()
{
	return max;
}
float MediaMovel::getMin()
{
	return min;
}
MediaMovel::operator float()
{
	return media;
}
*/

///Termometro
Term::Term(byte aPin) :Apin(aPin)
{
}
float Term::read()
{
	return (float(analogRead(Apin)) * 5 / (1023)) / 0.01;
}

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


///Controle de paraquedas
DuDeploy::DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay, float tMax) : P1(paraPin1), P2(paraPin2), I1(infPin1), I2(infPin2), Tign((unsigned long)(ignT * 1000000.0)), Delay((unsigned long)(delay * 1000000.0)), Tmax((unsigned long)(tMax * 1000000.0)), TmaxAux(tMax == 0 ? 0 : 1)
{
}
void DuDeploy::resetTimer()
{
	TimeZero = micros();
}
bool DuDeploy::info1()
{
	return digitalRead(I1);
}
bool DuDeploy::info2()
{
	return digitalRead(I2);
}
bool DuDeploy::begin()
{
	pinMode(P1, OUTPUT);
	pinMode(P2, OUTPUT);
	digitalWrite(P1, LOW);
	digitalWrite(P2, LOW);
	pinMode(I1, INPUT_PULLUP);
	pinMode(I2, INPUT_PULLUP);
	resetTimer();
	return info1() || info2();
}
void DuDeploy::setTmax(float Time)
{
	Tmax = (unsigned long)(Time * 1000000);
	TmaxAux = true;
}
void DuDeploy::setTdelay(float Time)
{
	TdelayAux = true;
	Tdelay = (unsigned long)(Time * 1000000);
}
void DuDeploy::setP1height(float H)
{
	P1H = H;
	P1H_A = true;
}
void DuDeploy::setP2height(float H)
{
	P2H = H;
	P2H_A = true;
}
void DuDeploy::sealApogee(bool apg)
{
	if (!apogee)
	{
		Tseal = micros() - TimeZero;
		apogee = apg;
	}
}
bool DuDeploy::getApogee()
{
	return apogee;
}
bool DuDeploy::getP1S(bool type)
{
	if (type) return P1S;
	else
	{
		if (!P1S_A && P1S)
		{
			P1S_A = 1;
			return 1;
		}
		else return 0;
	}
}
bool DuDeploy::getP2S(bool type)
{
	if (type) return P2S;
	else
	{
		if (!P2S_A && P2S)
		{
			P2S_A = 1;
			return 1;
		}
		else return 0;
	}
}
bool DuDeploy::getSysState(bool type)
{
	return (P1T_A && P2T_A && (P1S || P2S)) || (P2T_A && !P1S && (!P1T_A || P2S)) || (P1T_A && !P2S && (P1S || !P2T_A)) || (!type && !P1S && !P2S && (!P1T_A || !P2T_A));
}
void DuDeploy::refresh(float height)
{
	Tnow = micros() - TimeZero;
	if (TmaxAux && Tnow > Tmax && !apogee) sealApogee(true);	//Caso o tempo maximo seja exigido e tenha sido ultrapassado
	if (apogee && getSysState(0))	//Se o apogeu foi dado
	{
		if (TdelayAux)	//Somente caso o uso do atraso seja definido
		{
			if (!P1M_A)	//Salva somente uma vez
			{
				if (!P1H_A || (P1H_A && height <= P1H))	//Condicional base P1
				{
					P1M = Tnow;	//Momento de condicional verdadeira
					P1M_A = true;
				}
			}
			/*if (!P2M_A) //Salva somente uma vez
			{
				if ((!P2H_A && Tnow > P1T + Delay) || (P2H_A && height <= P2H))	//Condicional base P2
				{
					P2M = Tnow;	//Momento de condicional verdadeira
					P2M_A = true;
				}
			}*/
		}
		if (P1seal || (!P1H_A || (P1H_A && height <= P1H)) && !TdelayAux || TdelayAux && Tnow > P1M + Tdelay)	//Caso altura seja exigida, sera verificada
		{
			if (!P1seal) P1seal = 1;
			if (!P1T_A)	//Salvar tempo do acionamento de P1 somente uma vez
			{
				P1T_A = 1;
				P1T = Tnow;
			}
			if ((Tnow - P1T < Tign) && P1T_A) P1S = 1;	//Comando efetivo
			else P1S = 0;

		}
		if (P2seal || ((!P2H_A && Tnow > P1T + Delay) || (P2H_A && height <= P2H)) /* && !TdelayAux || TdelayAux && Tnow > P2M + Tdelay*/)	//Verifica tempo de atraso ou altura exigida
		{
			if (!P2seal) P2seal = 1;
			if (!P2T_A)	//Salva tempo do acionamento de P2 somente uma vez
			{
				P2T_A = 1;
				P2T = Tnow;
			}
			if (Tnow < P2T + Tign && P2T_A) P2S = 1;	//Comando efetivo
			else P2S = 0;
		}
	}
	digitalWrite(P1, P1S);
	digitalWrite(P2, P2S);
}
void DuDeploy::emergency(bool state)
{
	if (state && !emer)
	{
		P1H_Am = P1H_A;
		P2H_Am = P2H_A;
		P1H_A = false;
		P2H_A = false;
		sealApogee(true);
	}
	if (!state && emer)
	{
		P1H_A = P1H_Am;
		P2H_A = P2H_Am;
	}
	emer = state;
}

///Controle de acionamento unit�rio
bool MonoDeploy::apogee = false;
unsigned long MonoDeploy::TimeZero = 0;
unsigned long MonoDeploy::Tseal = 0;
float MonoDeploy::height = 0;
MonoDeploy::MonoDeploy(unsigned int commandPin, unsigned int infoPin, unsigned long actionTime, unsigned int systemMode): cPin(commandPin), iPin(infoPin), sysMode(systemMode), Tign((unsigned long)(actionTime * 1000000.0))
{
}
void MonoDeploy::resetTimer()
{
	TimeZero = micros();
}
void MonoDeploy::sealApogee(bool apg)
{
	if (!apogee)
	{
		Tseal = micros() - TimeZero;
		apogee = apg;
	}
}
bool MonoDeploy::getApogee()
{
	return apogee;
}
void MonoDeploy::putHeight(float H)
{
	height = H;
}
void MonoDeploy::setHeightCmd(float H)
{
	useH = true;
	cmdHeight = H;

}
void MonoDeploy::setDelayCmd(float T)
{
	useT = true;
	cmdDelay = (unsigned long)(T * 1000000.0f);
}
void MonoDeploy::setTmax(float T)
{
	useM = true;
	Tmax = (unsigned long)(T * 1000000.0f);
}
bool MonoDeploy::begin()
{
	if (sysMode == 0)
	{
		pinMode(cPin, OUTPUT);
		digitalWrite(cPin, sPin);
		pinMode(iPin, INPUT);
	}
#if SERVO_MODE
	else if(sysMode)
	{
		motor.attach(cPin);
		motor.write(0);
	}
#endif
	return info();
}
bool MonoDeploy::info()
{
	return analogRead(iPin) > 100;
}
bool MonoDeploy::getState(bool type)
{
	if (type) return sPin == command;
	else if (!stateAux && sPin == command)
	{
		stateAux = true;
		return true;
	}
	else return false;
}
bool MonoDeploy::getGlobalState()
{
	return globalState;
}
void MonoDeploy::refresh()
{
	Tnow = micros() - TimeZero;
	if (useM && Tnow >= TimeZero + Tmax && !getApogee()) sealApogee(true);
	if (getApogee())
	{
		if (useH && !useH_A)
		{
			if (height <= cmdHeight)
			{
				useH_A = true;
				Theight = Tnow;
			}
		}
		if (useT)
		{
			useT_A = (useH) ? (Tnow >= Theight + cmdDelay && useH_A) : (Tnow >= Tseal + cmdDelay);
		}
		bool triggers = (!useH && !useT) || (useH && !useT && useH_A) || (!useH && useT && useT_A) || (useH && useT && useH_A && useT_A);
		if (cmdSeal || triggers)
		{
			if (!cmdSeal)
			{
				cmdSeal = true;
				Tcmd = Tnow;
			}
			if ((Tnow - Tcmd < Tign) && cmdSeal) sPin = command;
			else sPin = !command;
		}
	}
	if(sysMode == 0) digitalWrite(cPin, sPin);
#if SERVO_MODE
	else if(sysMode == 1)
	{
		if (sPin) motor.write(90);
		else motor.write(0);
	}
#endif
	if (!globalStateAux && sPin == command)	globalStateAux = true;
	else if (globalStateAux && sPin != command)	globalState = false;
}


///Auxiliar para o uso do cartao SD
SDCH::SDCH(uint8_t csPin, String name, String type) :CS(csPin), Fname0(name), Ftype((type.length() == 3) ? type : (String)"txt"), coef(8 - name.length()), nMax(pow(10, (name.length() < 8) ? 8 - name.length() : 0))
{
	newName();
}
void SDCH::newName()
{
	Fname = Fname0;
	if (coef > 0)
	{
		if (String(number).length() > coef) number = 0;
		String n = String(number);
		Fname = Fname0;
		for (int i = coef - n.length(); i > 0; i--)	Fname += '0';
		Fname += n;
		number++;
	}
	else
	{
		String aux = Fname;
		Fname = "";
		for (int i = 0; i < 8; i++) Fname += aux[i];
	}
	Fname += '.' + Ftype;
}
bool SDCH::begin()
{
	if (!SD.begin(CS)) return false;
	else
	{ //Inicializa o SD
		for (unsigned long int i = 0; i < nMax; i++)
		{ //Cria um arquivo com nome novo
			if (!SD.exists(Fname)) break;
			else newName();
		}
		theFile = SD.open(Fname, FILE_WRITE);
		if (theFile)
		{
			theFile.close();
			return true;
		}
		else return false;
	}
}
bool SDCH::open()
{
	return theFile = SD.open(Fname, FILE_WRITE);
}
size_t SDCH::tab()
{
	return theFile.write('\t');
}
void SDCH::close()
{
	theFile.close();
}
SDCH::operator bool()
{
	return open();
}
String SDCH::getFname()
{
	return Fname;
}

/*
ComProtocol::ComProtocol(HardwareSerial &S, unsigned long baud) : Com(S), baudRate(baud)
{
}
void ComProtocol::begin()
{
	Com.begin(baudRate);
}
void ComProtocol::inform(information info)
{
	if (info == none) return;
	Com.print(F("@CP"));	//At ComProtocol
	switch (info)
	{
	case apgDeteced: Com.print("AD"); break;
	case apgHeightV: Com.print("AH"); return;
	case apgMomentV: Com.print("AM"); return;
	case FirstDNorm: Com.print("FN"); break;
	case SecndDNorm: Com.print("SN"); break;
	case FirstDBack: Com.print("FB"); break;
	case SecndDBack: Com.print("SB"); break;
	case WUFDrctvOf: Com.print("WU"); break;
	case SystemBoot: Com.print("BS"); break;
	default: Com.print("*");
	}
	Com.print(F("$"));
}
void ComProtocol::informInteger(information info, long rawData)
{

	switch (info)
	{
	case apgMomentV:
		inform(info);
		Com.print(F("#"));
		Com.print(rawData);
		Com.print(F("$"));
		break;
	default: return;
	}
}
void ComProtocol::informFloating(information info, float rawData)
{
	switch (info)
	{
	case apgHeightV:
		inform(info);
		Com.print(F("#"));
		Com.print(rawData);
		Com.print(F("$"));
		break;
	default: return;
	}
}
ComProtocol::information ComProtocol::understand()
{
	successful = false;
	byte counter = 1;
	start: if (Com.available() < 5 || counter > 5) return none;
	if (Com.peek() == '\0')
	{
		counter++;
		Com.read();
		goto start;
	}
	char cptcl[3];
	String ptcl("@CP");
	Com.readBytes(cptcl, 3);
	if (ptcl.equals(ptcl))
	{
		switch (Com.read())	//Code tag is about, then tag is for
		{
		case 'A':	//Apogee
			switch (Com.read())
			{
			case 'D': if (Com.read() == '$') successful = true; return apgDeteced;	//Detected
			case 'H':
				if (Com.read() == '#') lastFloating = Com.parseFloat();
				if (Com.read() == '$') successful = true;
				return apgHeightV;	//Height
			case 'M':
				if (Com.read() == '#') lastInteger = Com.parseInt();
				if (Com.read() == '$') successful = true;
				return apgMomentV;	//Moment
			default: return none;
			}
		case 'B': 	//Boot
			switch (Com.read())
			{
			case 'S': if (Com.read() == '$') successful = true; return SystemBoot; 	//System
			default: return none;
			}
		case 'F':	//First deploy
			switch (Com.read())
			{
			case 'B': if (Com.read() == '$') successful = true; return FirstDBack;	//Backup
			case 'N': if (Com.read() == '$') successful = true; return FirstDNorm;	//Normal
			default: return none;
			}
		case 'S':	//Second deploy
			switch (Com.read())
			{
			case 'B': if (Com.read() == '$') successful = true; return SecndDBack;	//Backup
			case 'N': if (Com.read() == '$') successful = true; return SecndDNorm;	//Normal
			default: return none;
			}
		case 'W':	//Waiters
			switch (Com.read())
			{
			case 'U': if (Com.read() == '$') successful = true; return WUFDrctvOf;	//Wait Until Flight
			default: return none;
			}
		default: return none;
		}
	}
	return none;
}
long ComProtocol::receiveInteger()
{
	return lastInteger;
}
float ComProtocol::receiveFloating()
{
	return lastFloating;
}
*/