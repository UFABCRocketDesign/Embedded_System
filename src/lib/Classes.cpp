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