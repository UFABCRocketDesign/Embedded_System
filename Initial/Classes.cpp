#include "Classes.h"

///Sensor base class
Sens::Sens(uint8_t add, long recalT) : address(add), recalibrateT((long)(recalT * 1000000))
{
}
long Sens::getTimeLapse()
{
	return lastReadT - lastWorkT;
}
Sens::operator bool()
{
	return readAll();
}


///Barometro
Baro::Baro(float recalT) :Sens(0x77, recalT)
{
}
void Baro::begin()
{
	Wire.beginTransmission(address);
	Wire.write(0XAA);
	Wire.endTransmission();

	Wire.requestFrom(address, (uint8_t)22);
	unsigned long temp = micros();
	while (Wire.available() < 22)
	{
		if (temp + 10 < micros())break;
	}
	ac1 = Wire.read() << 8 | Wire.read();
	ac2 = Wire.read() << 8 | Wire.read();
	ac3 = Wire.read() << 8 | Wire.read();
	ac4 = Wire.read() << 8 | Wire.read();
	ac5 = Wire.read() << 8 | Wire.read();
	ac6 = Wire.read() << 8 | Wire.read();
	b1 = Wire.read() << 8 | Wire.read();
	b2 = Wire.read() << 8 | Wire.read();
	mb = Wire.read() << 8 | Wire.read();
	mc = Wire.read() << 8 | Wire.read();
	md = Wire.read() << 8 | Wire.read();
}
bool Baro::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(address);
	state = Wire.endTransmission() == 0;
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
#if PRINT
			Serial.println(F("Relacibrado B"));
#endif // PRINT
		}
		//Leitura de temperatura

		// Write 0x2E into Register 0xF4
		// This requests a temperature reading
		Wire.beginTransmission(address);
		Wire.write(0xF4);
		Wire.write(0x2E);
		Wire.endTransmission();

		// Wait at least 4.5ms
		delayMicroseconds(4550);

		// Read two bytes from registers 0xF6 and 0xF7
		Wire.beginTransmission(address);
		Wire.write(0xF6);
		Wire.endTransmission();

		Wire.requestFrom(address, (uint8_t)2);
		unsigned long temp = micros();
		while (Wire.available() < 2)
		{
			if (temp + 10 < micros())break;
		}
		ut = Wire.read() << 8 | Wire.read();

		//Leitura de pressao

		// Write 0x34+(OSS<<6) into register 0xF4
		// Request a pressure reading w/ oversampling setting
		Wire.beginTransmission(address);
		Wire.write(0xF4);
		Wire.write(0x34 + (OSS << 6));
		Wire.endTransmission();

		// Wait for conversion, delay time dependent on OSS
		delay(2 + (3 << OSS));

		// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
		Wire.beginTransmission(address);
		Wire.write(0xF6);
		Wire.endTransmission();

		Wire.requestFrom(address, (uint8_t)3);
		temp = micros();
		while (Wire.available() < 3)
		{
			if (temp + 10 < micros())break;
		}

		up = (((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | (unsigned long)Wire.read()) >> (8 - OSS);

		//Garante que todas as leituras foram feitas
		Wire.beginTransmission(address);
		state = Wire.endTransmission() == 0;
		if (state)
		{

			//Calculo de temperatura
			b5 = ((((long)ut - (long)ac6)*(long)ac5) >> 15) + ((long)mc << 11) / (((((long)ut - (long)ac6)*(long)ac5) >> 15) + md);
			celcius = (float)((b5 + 8) >> 4) / 10;

			//Calculo de pressao
			b6 = b5 - 4000;
			// Calcula B3
			x1 = (b2 * (b6 * b6) >> 12) >> 11;
			x2 = (ac2 * b6) >> 11;
			x3 = x1 + x2;
			b3 = (((((long)ac1) * 4 + x3) << OSS) + 2) >> 2;

			// Calcula B4
			x1 = (ac3 * b6) >> 13;
			x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
			x3 = ((x1 + x2) + 2) >> 2;
			b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

			b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
			if (b7 < 0x80000000) pascal = (b7 << 1) / b4;
			else pascal = (b7 / b4) << 1;

			x1 = (pascal >> 8) * (pascal >> 8);
			x1 = (x1 * 3038) >> 16;
			x2 = (-7357 * pascal) >> 16;
			pascal += (x1 + x2 + 3791) >> 4;

			lastWorkT = thisReadT;
		}
	}
	lastReadT = thisReadT;
	return state;
}
float Baro::getTemperature()
{
	return celcius;
}
long Baro::getPressure()
{
	return pascal;
}


///Triaxial
float TriA::getX()
{
	return X;
}
float TriA::getY()
{
	return Y;
}
float TriA::getZ()
{
	return Z;
}


///Acelerometro
Acel::Acel(long recalT) :Sens(0x53, recalT)
{
}
void Acel::begin()
{
	Wire.beginTransmission(address);// enable to measute g data
	Wire.write(0x2D);
	Wire.write(8);                    //measuring enable
	Wire.endTransmission();           // stop transmitting

	Wire.beginTransmission(address);// enable to measute g data
	Wire.write(0x31);
	Wire.endTransmission();
	Wire.requestFrom(address, (uint8_t)1);
	unsigned long temp = micros();
	while (!Wire.available())
	{
		if (temp + 10 < micros())break;
	}
	char B = Wire.read();
	B &= ~0x0F;
	B |= 0b11;
	B |= 8;
	Wire.beginTransmission(address);
	Wire.write(0x31);
	Wire.write(B);                    //measuring enable
	Wire.endTransmission();           // stop transmitting
}
bool Acel::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(address);
	state = Wire.endTransmission() == 0;
	///Faz a leitura de todos os eixos///
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
#if PRINT
			Serial.println(F("Relacibrado A"));
#endif // PRINT
		}
		Wire.beginTransmission(address);
		Wire.write(0x32);							//Xlsb register
		Wire.endTransmission();
		Wire.requestFrom(address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() | Wire.read() << 8;		//X_lsb + X_msb << 8
			Y = Wire.read() | Wire.read() << 8;		//Y_lsb + Y_msb << 8
			Z = Wire.read() | Wire.read() << 8;		//Z_lsb + Z_msb << 8
		}
		X *= 0.004 * 9.80665F;
		Y *= 0.004 * 9.80665F;
		Z *= 0.004 * 9.80665F;

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}


///Magnetometro
Magn::Magn(long recalT) :Sens(0x1E, recalT)
{
}
void Magn::begin()
{
	Wire.beginTransmission(address);
	Wire.write(0x02);                 // Seleciona o modo
	Wire.write(0x00);                 // Modo de medicao continuo
	Wire.endTransmission();
}
bool Magn::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(address);
	state = Wire.endTransmission() == 0;
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
#if PRINT
			Serial.println(F("Relacibrado M"));
#endif // PRINT
		}
		///Faz a leitura de todos os eixos///
		Wire.beginTransmission(address);
		Wire.write(0x03);							//Xmsb register
		Wire.endTransmission();
		Wire.requestFrom(address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() << 8 | Wire.read();		//X_msb << 8 | X_lsb
			Z = Wire.read() << 8 | Wire.read();		//Z_msb << 8 | Z_lsb
			Y = Wire.read() << 8 | Wire.read();		//Y_msb << 8 | Y_lsb
		}
		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}


///Giroscopio
Giro::Giro(long sc, long recalT) : scale(sc), Sens(0x69, recalT)
{
}
void Giro::begin()
{
	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x20);							// send register address ctrl_1
	Wire.write(0b00001111);						// send value to write
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x21);							// send register address ctrl_2
	Wire.write(0b00000000);						// send value to write
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x22);							// send register address ctrl_3
	Wire.write(0b00001000);						// send value to write
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x23);							// send register address ctrl_4
	switch (scale)
	{ // Configura o L3G4200 para 200, 500 ou 2000 graus/seg
	case 250: Wire.write(0b00000000); break;	// send value to write
	case 500: Wire.write(0b00010000); break;	// send value to write
	default:  Wire.write(0b00110000); break;	// send value to write
	}
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x24);							// send register address ctrl_5
	Wire.write(0b00000000);						// send value to write
	Wire.endTransmission();						// end transmission
}
bool Giro::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(address);
	state = Wire.endTransmission() == 0;
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
#if PRINT
			Serial.println(F("Relacibrado G"));
#endif // PRINT
		}
		///Faz a leitura de todos os eixos///
		Wire.beginTransmission(address);
		Wire.write(0x28 | (1 << 7));				//Xlsb register
		Wire.endTransmission();
		Wire.requestFrom(address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() | (Wire.read() << 8);
			Y = Wire.read() | (Wire.read() << 8);
			Z = Wire.read() | (Wire.read() << 8);
		}
		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}

///Coleaao de utilitarios
void Helpful::begin()
{
	T0 = micros();
}
unsigned long Helpful::counter()
{
	return count++;
}
unsigned long Helpful::getCount()
{
	return count;
}
unsigned long Helpful::counterReset()
{
	unsigned long C = count;
	count = 0;
	return  C;
}
bool Helpful::eachN(unsigned int N)
{
	conEach++;
	return (N == 0) ? false : conEach%N == 0;
}
bool Helpful::eachT(float T)
{
	if (micros() - timerEach > (unsigned long)(T*1000000.0))
	{
		timerEach = micros();
		return true;
	}
	return false;
}
void Helpful::forT(float T)
{
	endT = micros() + (long)(T*1000000.0);
	forTstate = 1;
}
bool Helpful::forT()
{
	if (micros() > endT && forTstate)
	{
		endT = 0;
		return forTstate = 0;
	}
	return forTstate;
}
float Helpful::lapse()
{
	unsigned long tnow = micros(), t = tnow - lapseT;
	lapseT = tnow;
	return (float)t / 1000000.0;
}
float Helpful::sinceBegin()
{
	return float(micros() - T0) / 1000000.0;
}
bool Helpful::oneTime()
{
	if (!one)
	{
		one = 1;
		return true;
	}
	return false;
}
void Helpful::oneTimeReset()
{
	if (one) one = 0;
}
void Helpful::comparer(float n)
{
	maxi = (n > maxi) ? n : maxi;
	mini = (n < mini) ? n : mini;
}
float Helpful::getMax()
{
	return maxi;
}
float Helpful::getMin()
{
	return mini;
}


///Filtro de media movel
MediaMovel::MediaMovel(int n) : N(n)
{
	for (unsigned int i = 0; i < N; i++) Vals[i] = 0;
}
MediaMovel::~MediaMovel()
{
	delete[] Vals;
}
float MediaMovel::addValor(float valor)
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


/*
///Termometro
Term::Term(byte aPin) :Apin(aPin)
{
}
float Term::read()
{
return (float(analogRead(Apin)) * 5 / (1023)) / 0.01;
}
*/


///Dados do GPS
GyGPS::GyGPS(HardwareSerial S, short gmt) :GpSerial(S), GMT(gmt), Sens(NULL)
{
}
unsigned short GyGPS::getFailed()
{
	return failed;
}
unsigned short GyGPS::getSentences()
{
	return sentences;
}
unsigned char GyGPS::getSatellites()
{
	return satellites;
}
unsigned long GyGPS::getPrecision()
{
	return precision;
}
unsigned long GyGPS::getChars()
{
	return chars;
}
unsigned long GyGPS::getAge()
{
	return age;
}
float GyGPS::getLatitude()
{
	return latitude;
}
float GyGPS::getLongitude()
{
	return longitude;
}
float GyGPS::getAltitude()
{
	return altitude;
}
float GyGPS::getKph()
{
	return Kph;
}
float GyGPS::getMps()
{
	return mps;
}
int GyGPS::getYear()
{
	return year;
}
byte GyGPS::getMonth()
{
	return month;
}
byte GyGPS::getDay()
{
	return day;
}
byte GyGPS::getHour(bool gmt)
{
	return hour + gmt ? GMT : 0;
}
byte GyGPS::getMinute()
{
	return minute;
}
byte GyGPS::getSecond()
{
	return second;
}
void GyGPS::begin()
{
	GpSerial.begin(9600);
}
bool GyGPS::readAll()
{
	thisReadT = micros();
	state = false;
	// Recebe as informacoes do GPS durante um intervalo de tempo relativamente curto e as transmite via comunicacao serial (visivel pelo serial monitor)
	if (GpSerial.available()) while (GpSerial.available())
	{
		char c = GpSerial.read();
		state = gps.encode(c);
	}
	if (state)
	{
		gps.f_get_position(&latitude, &longitude, &age);
		satellites = gps.satellites();
		precision = gps.hdop();
		altitude = gps.f_altitude();
		Kph = gps.f_speed_kmph();
		mps = gps.f_speed_mps();

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	gps.crack_datetime(&year, &month, &day, &hour, &minute, &second);
	gps.stats(&chars, &sentences, &failed);
	return state;
}
bool GyGPS::isNew()
{
	return state;
}


///Rotinas de verificacao de apogeu
Apogeu::Apogeu(unsigned int n, unsigned int r, float s) : N(n), R((r > 1) ? r : 2), Rl1((r > 1) ? r - 1 : 1), S(s)
{
	Rf = 0;
	for (unsigned int i = 0; i < R; i++) altMed[i] = 0;
	for (unsigned int i = 0; i < N;i++) Alt[i] = 0;
	for (int i = Rl1; i > 0; i--)
	{
		Rf += (float)(i*i);
	}
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
float Apogeu::calcAlt(long P, float sealevelP)
{
	for (int i = N - 1; i > 0; i--)
	{
		Alt[i] = Alt[i - 1];
	}
	Alt[0] = ((1 - pow((float)P / sealevelP, 1 / 5.25588)) / 0.0000225577) - base;
	float Sum = 0;
	for (uint8_t i = 0; i < N; i++)
	{
		Sum += Alt[i];
	}
	for (uint8_t i = Rl1; i > 0; i--)
	{
		altMed[i] = altMed[i - 1];
	}
	altMed[0] = Sum / N;
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
	if (!apogee) apogee = apg;
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
	if (TmaxAux && Tnow > Tmax && !apogee) apogee = true;	//Caso o tempo maximo seja exigido e tenha sido ultrapassado
	if (apogee && getSysState(0))	//Se o apogeu foi dado
	{
		if (!P1H_A || (P1H_A && height <= P1H) || P1seal)	//Caso altura seja exigida, sera verificada
		{
			if (!P1seal) P1seal = 1;
			if (!P1T_A)	//Salvar tempo de P1 somente uma vez
			{
				P1T_A = 1;
				P1T = Tnow;
			}
			if ((Tnow - P1T < Tign) && P1T_A) P1S = 1;	//Comando efetivo
			else P1S = 0;

		}
		if ((!P2H_A && Tnow > P1T + Delay) || (P2H_A && height <= P2H) || P2seal)	//Verifica tempo de atraso ou altura exigida
		{
			if (!P2seal) P2seal = 1;
			if (!P2T_A)	//Salva tempo de P2 somente uma vez
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
		apogee = true;
	}
	if (!state && emer)
	{
		P1H_A = P1H_Am;
		P2H_A = P2H_Am;
	}
	emer = state;
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