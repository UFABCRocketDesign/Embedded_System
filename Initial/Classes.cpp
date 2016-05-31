#include "Classes.h"


///Coleção de utilitários
void Helpful::begin()
{
	T0 = micros();
}
long Helpful::counter()
{
	return count++;
}
long Helpful::getCount()
{
	return count;
}
bool Helpful::eachN(unsigned int N)
{
	conEach++;
	return (N == 0) ? false : conEach%N == 0;
}
bool Helpful::eachT(float T)
{
	unsigned long time = (long)(T*1000000.0);
	if (micros() - timerEach > time)
	{
		timerEach = micros();
		return true;
	}
	return false;
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


///Filtro de m�dia m�vel
MediaMovel::MediaMovel(int n) : N(n)
{
	for (unsigned int i = 0; i < N; i++) Vals[i] = 0;
}
float MediaMovel::addValor(float valor)
{
	for (byte i = N - 1; i > 0; i--)
	{
		Vals[i] = Vals[i - 1];
	}
	Vals[0] = valor;
	media = 0;
	for (byte i = 0; i < N; i++)
	{
		media += Vals[i];
	}
	media /= N;
	if (media > max) max = media;
	if (media < min) min = media;
	return media;
}
float MediaMovel::getMedia()
{
	return media;
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


///Barômetro
Baro::Baro(float Tzero) :recalibrateT((long)(Tzero * 1000000))
{
}

char Baro::ReadChar(unsigned char address)
{
	Wire.beginTransmission(BMP085_Address);
	Wire.write(address);
	Wire.endTransmission();

	Wire.requestFrom(BMP085_Address, 1);
	unsigned long temp = micros();
	while (!Wire.available())
	{
		if (temp + 10 < micros())break;
	}
	return Wire.read();
}
int Baro::ReadInt(unsigned char address)
{
	unsigned char msb, lsb;

	Wire.beginTransmission(BMP085_Address);
	Wire.write(address);
	Wire.endTransmission();

	Wire.requestFrom(BMP085_Address, 2);
	unsigned long temp = micros();
	while (Wire.available() < 2)
	{
		if (temp + 10 < micros())break;
	}
	msb = Wire.read();
	lsb = Wire.read();

	return (int)msb << 8 | lsb;
}
float Baro::readTemperature()
{

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	Wire.beginTransmission(BMP085_Address);
	Wire.write(0xF4);
	Wire.write(0x2E);
	Wire.endTransmission();

	// Wait at least 4.5ms
	delayMicroseconds(4550);

	// Read two bytes from registers 0xF6 and 0xF7
	ut = ReadInt(0xF6);

	b5 = ((((long)ut - (long)ac6)*(long)ac5) >> 15) + ((long)mc << 11) / (((((long)ut - (long)ac6)*(long)ac5) >> 15) + md);
	celcius = (float)((b5 + 8) >> 4) / 10;
	return celcius;
}
long Baro::readPressure()
{
	// Write 0x34+(OSS<<6) into register 0xF4
	// Request a pressure reading w/ oversampling setting
	Wire.beginTransmission(BMP085_Address);
	Wire.write(0xF4);
	Wire.write(0x34 + (OSS << 6));
	Wire.endTransmission();

	// Wait for conversion, delay time dependent on OSS
	delay(2 + (3 << OSS));

	// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
	msb = ReadChar(0xF6);
	lsb = ReadChar(0xF7);
	xlsb = ReadChar(0xF8);

	up = (((unsigned long)msb << 16) | ((unsigned long)lsb << 8) | (unsigned long)xlsb) >> (8 - OSS);

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
	if (b7 < 0x80000000)
		p = (b7 << 1) / b4;
	else
		p = (b7 / b4) << 1;

	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p += (x1 + x2 + 3791) >> 4;

	pascal = p;
	return pascal;
}
float Baro::readAltitude(float sealevelP)
{
	meters = ((1 - pow((float)pascal / sealevelP, 1 / 5.25588)) / 0.0000225577);
	return meters;
}

void Baro::begin()
{
	ac1 = ReadInt(0xAA);
	ac2 = ReadInt(0xAC);
	ac3 = ReadInt(0xAE);
	ac4 = ReadInt(0xB0);
	ac5 = ReadInt(0xB2);
	ac6 = ReadInt(0xB4);
	b1 = ReadInt(0xB6);
	b2 = ReadInt(0xB8);
	mb = ReadInt(0xBA);
	mc = ReadInt(0xBC);
	md = ReadInt(0xBE);
}
long Baro::getTimeLapse()
{
	return lastReadT - lastWorkT;
}
bool Baro::readAll(float sealevelP)
{
	thisReadT = micros();
	Wire.beginTransmission(BMP085_Address);
	state = Wire.endTransmission() == 0;
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
			Serial.println("Relacibrado B");
		}
		//Leitura de temperatura

		// Write 0x2E into Register 0xF4
		// This requests a temperature reading
		Wire.beginTransmission(BMP085_Address);
		Wire.write(0xF4);
		Wire.write(0x2E);
		Wire.endTransmission();

		// Wait at least 4.5ms
		delayMicroseconds(4550);

		// Read two bytes from registers 0xF6 and 0xF7
		ut = ReadInt(0xF6);

		//Leitura de pressão

		// Write 0x34+(OSS<<6) into register 0xF4
		// Request a pressure reading w/ oversampling setting
		Wire.beginTransmission(BMP085_Address);
		Wire.write(0xF4);
		Wire.write(0x34 + (OSS << 6));
		Wire.endTransmission();

		// Wait for conversion, delay time dependent on OSS
		delay(2 + (3 << OSS));

		// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
		msb = ReadChar(0xF6);
		lsb = ReadChar(0xF7);
		xlsb = ReadChar(0xF8);

		//Garante que todas as leituras foram feitas
		Wire.beginTransmission(BMP085_Address);
		state = Wire.endTransmission() == 0;
		if (state)
		{

			//Calculo de temperatura

			b5 = ((((long)ut - (long)ac6)*(long)ac5) >> 15) + ((long)mc << 11) / (((((long)ut - (long)ac6)*(long)ac5) >> 15) + md);
			celcius = (float)((b5 + 8) >> 4) / 10;

			//Calculo de pressão
			up = (((unsigned long)msb << 16) | ((unsigned long)lsb << 8) | (unsigned long)xlsb) >> (8 - OSS);

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
			if (b7 < 0x80000000)
				p = (b7 << 1) / b4;
			else
				p = (b7 / b4) << 1;

			x1 = (p >> 8) * (p >> 8);
			x1 = (x1 * 3038) >> 16;
			x2 = (-7357 * p) >> 16;
			p += (x1 + x2 + 3791) >> 4;

			pascal = p;

			//Calculo de altitude
			meters = ((1 - pow((float)pascal / sealevelP, 1 / 5.25588)) / 0.0000225577);

			lastWorkT = thisReadT;
		}
	}
	lastReadT = thisReadT;
	return state;
}
float Baro::readZero(unsigned int I)
{
	float Regis = 0;
	for (byte i = 0; i < I; i++)
	{
		readAll();
		Regis += meters;
	}
	base = (I > 0) ? Regis / I : 0;
	return base;
}
long Baro::readSealevel(float altitude)
{
	readAll();
	return (long)(pascal / pow(1.0 - altitude*0.0000225577, 5.25588));
}
float Baro::getZero()
{
	return base;
}
float Baro::getTemperature()
{
	return celcius;
}
long Baro::getPressure()
{
	return pascal;
}
float Baro::getAltitude(int Base)
{
	return meters - Base;
}
float Baro::getAltitude()
{
	return meters - base;
}
Baro:: operator bool()
{
	return readAll();
}


///Magnet?metro
Mag::Mag(float Tzero) :recalibrateT((long)(Tzero * 1000000))
{
}
void Mag::begin()
{
	Wire.beginTransmission(HMC5883_Address);
	Wire.write(0x02);                 // Seleciona o modo
	Wire.write(0x00);                 // Modo de medicao continuo
	Wire.endTransmission();
}
long Mag::getTimeLapse()
{
	return lastReadT - lastWorkT;
}
bool Mag::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(HMC5883_Address);
	state = Wire.endTransmission() == 0;
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
			Serial.println("Relacibrado M");
		}
		///Faz a leitura de todos os eixos///
		Wire.beginTransmission(HMC5883_Address);
		Wire.write(Mag_Xmsb);
		Wire.endTransmission();
		Wire.requestFrom(HMC5883_Address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() << 8 | Wire.read(); //X msb
			//X += Wire.read();    //X lsb
			Z = Wire.read() << 8 | Wire.read(); //Z msb
			//Z += Wire.read();    //Z lsb
			Y = Wire.read() << 8 | Wire.read(); //Y msb
			//Y += Wire.read();    //Y lsb
		}
		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}
float Mag::getX()
{
	return X;
}
float Mag::getY()
{
	return Y;
}
float Mag::getZ()
{
	return Z;
}
Mag:: operator bool()
{
	return readAll();
}


///Girosc?pio
Giro::Giro(int sc, float Tzero) : scale(sc), recalibrateT((long)(Tzero * 1000000))
{

}
void Giro::begin()
{
	Wire.beginTransmission(L3G4200D_Address); // start transmission to device
	Wire.write(CTRL_REG1);    // send register address
	Wire.write(0b00001111);   // send value to write
	Wire.endTransmission();   // end transmission

	Wire.beginTransmission(L3G4200D_Address); // start transmission to device
	Wire.write(CTRL_REG2);    // send register address
	Wire.write(0b00000000);   // send value to write
	Wire.endTransmission();   // end transmission

	Wire.beginTransmission(L3G4200D_Address); // start transmission to device
	Wire.write(CTRL_REG3);       // send register address
	Wire.write(0b00001000);         // send value to write
	Wire.endTransmission();     // end transmission

	Wire.beginTransmission(L3G4200D_Address); // start transmission to device
	Wire.write(CTRL_REG4);       // send register address
	switch (scale)
	{ // Configura o L3G4200 para 200, 500 ou 2000 graus/seg
	case 250:
		Wire.write(0b00000000);         // send value to write
		break;
	case 500:
		Wire.write(0b00010000);         // send value to write
		break;
	default:
		Wire.write(0b00110000);         // send value to write
		break;
	}
	Wire.endTransmission();     // end transmission

	Wire.beginTransmission(L3G4200D_Address); // start transmission to device
	Wire.write(CTRL_REG5);       // send register address
	Wire.write(0b00000000);         // send value to write
	Wire.endTransmission();     // end transmission
}
long Giro::getTimeLapse()
{
	return lastReadT - lastWorkT;
}
bool Giro::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(L3G4200D_Address);
	state = Wire.endTransmission() == 0;
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
			Serial.println("Relacibrado G");
		}
		///Faz a leitura de todos os eixos///
		Wire.beginTransmission(L3G4200D_Address);
		Wire.write(Gyro_Xlsb | (1 << 7));
		Wire.endTransmission();
		Wire.requestFrom(L3G4200D_Address, (uint8_t)6);
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
float Giro::getX()
{
	return X;
}
float Giro::getY()
{
	return Y;
}
float Giro::getZ()
{
	return Z;
}
Giro:: operator bool()
{
	return readAll();
}


///Aceler?metro
Acel::Acel(float Tzero) :recalibrateT((long)(Tzero * 1000000))
{
}
void Acel::begin()
{
	Wire.beginTransmission(ADXL345_Address);// enable to measute g data
	Wire.write(Register_2D);
	Wire.write(8);                    //measuring enable
	Wire.endTransmission();           // stop transmitting

	Wire.beginTransmission(ADXL345_Address);// enable to measute g data
	Wire.write(Register_31);
	Wire.endTransmission();
	Wire.requestFrom(ADXL345_Address, (uint8_t)1);
	unsigned long temp = micros();
	while (!Wire.available())
	{
		if (temp + 10 < micros())break;
	}
	char B = Wire.read();
	B &= ~0x0F;
	B |= 0b11;
	B |= 8;
	Wire.beginTransmission(ADXL345_Address);
	Wire.write(Register_31);
	Wire.write(B);                    //measuring enable
	Wire.endTransmission();           // stop transmitting
}
long Acel::getTimeLapse()
{
	return lastReadT - lastWorkT;
}
bool Acel::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(ADXL345_Address);
	state = Wire.endTransmission() == 0;
	///Faz a leitura de todos os eixos///
	if (state)
	{
		if (getTimeLapse() > recalibrateT)
		{
			begin();
			Serial.println("Relacibrado A");
		}
		Wire.beginTransmission(ADXL345_Address);
		Wire.write(Acel_Xlsb);
		Wire.endTransmission();
		Wire.requestFrom(ADXL345_Address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() | Wire.read() << 8;    //X lsb
			//X += Wire.read() << 8;  //X msb
			Y = Wire.read() | Wire.read() << 8;    //Y lsb
			//Y += Wire.read() << 8;  //Y msb
			Z = Wire.read() | Wire.read() << 8;    //Z lsb
			//Z += Wire.read() << 8;  //Z msb
		}
		X *= 0.004 * 9.80665F;
		Y *= 0.004 * 9.80665F;
		Z *= 0.004 * 9.80665F;

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}
float Acel::getX()
{
	return X;
}
float Acel::getY()
{
	return Y;
}
float Acel::getZ()
{
	return Z;
}
Acel:: operator bool()
{
	return readAll();
}


///Rotinas de verifica��o de apogeu
Apogeu::Apogeu(unsigned int n, unsigned int r, float s) : N(n), R(r), Rl1(r - 1), S(s)
{
	Rf = 0;
	for (unsigned int i = 0; i < R; i++) altMed[i] = 0;
	for (unsigned int i = 0; i < N;i++) Alt[i] = 0;
	for (int i = Rl1; i > 0; i--)
	{
		Rf += (float)(i*i);
	}
}
void Apogeu::resetTimer()
{
	TimeZero = micros();
}
float Apogeu::addAltitude(float H)
{
	for (int i = N - 1; i > 0; i--)
	{
		Alt[i] = Alt[i - 1];
	}
	Alt[0] = H;
	float Sum = 0;
	for (uint8_t i = 0; i < N; i++)
	{
		Sum += Alt[i];
	}
	for (uint8_t i = Rl1 - 1; i > 0; i--)
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
void Apogeu::setOmega(bool apgE)
{
	Gamma = apgE;
}

bool Apogeu::apgAlpha()
{
	Alpha = 1;
	for (byte i = Rl1 - 1; i > 0; i--)
	{
		if (altMed[i] > altMed[i - 1]) Alpha &= 1;
		else Alpha &= 0;
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
		}
	}
	if (serial) for (int i = Rl1 - 1; i >= 0; i--) Serial.print(cond[i]);
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

float Apogeu::getAltutude()
{
	return altMed[0];
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
DuDeploy::DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay) : P1(paraPin1), P2(paraPin2), I1(infPin1), I2(infPin2), Tign((long)(ignT * 1000000.0)), Delay((long)(delay * 1000000.0))
{
	pinMode(P1, OUTPUT);
	pinMode(P2, OUTPUT);
	pinMode(I1, INPUT_PULLUP);
	pinMode(I2, INPUT_PULLUP);
	digitalWrite(P1, LOW);
	digitalWrite(P2, LOW);
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
	return (P1T_A && P2T_A && (P1S || P2S)) || (P2T_A && !P1S && (!P1T_A || P2S)) || (P1T_A && !P2S && (P1S || !P2T_A)) || (!type && !P1S && !P2S && (!P1T_A || !P1T_A));
}
void DuDeploy::refresh(float height)
{
	Tnow = micros() - TimeZero;
	if (TmaxAux && Tnow > Tmax && !apogee) apogee = true;	//Caso o tempo máximo seja exigido e tenha sido ultrapassado
	if (apogee && getSysState(0))	//Se o apogeu foi dado
	{
		if (!P1H_A || (P1H_A && height <= P1H) || P1seal)	//Caso altura seja exigida, será verificada
		{
			if (!P1seal) P1seal = 1;
			if (!P1T_A)	//Salvar tempo de P1 somente uma vez
			{
				P1T_A = 1;
				P1T = Tnow;
			}
			if ((Tnow - P1T < Tign) && P1T_A)
			{
				P1S = 1;	//Comando efetivo

			}
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


///Auxiliar para o uso do cartão SD
SDCardHelper::SDCardHelper(uint8_t csPin, String name, String type, float Tzero) :CS(csPin), Fname0(name), Ftype((type.length() > 3) ? ((String)type[0] + (String)type[1] + (String)type[2]) : (type.length() == 3) ? type : (type.length() == 2) ? type + (String)"0" : (type.length() == 1) ? type + (String)"00" : (String)"txt"), recalibrateT((long)(Tzero * 1000000))
{
	coef = 8 - Fname0.length();
	nMax = pow(10, (coef > 0) ? coef : 0);
	newName();
}

void SDCardHelper::newName()
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

bool SDCardHelper::begin(bool type)
{
	Serial.println("SDbegin");
	if (!SD.begin(CS))
	{
		Serial.println("SDFail");
		return false;
	}
	else
	{ //Inicializa o SD
		for (unsigned long int i = 0; i < nMax; i++)
		{ //Cria um arquivo com nome novo
			if (!SD.exists(Fname)) break;
			else newName();
		}
		file = SD.open(Fname, FILE_WRITE);
		if (file)
		{
			println("temp\t\tacel\t\t\tgiro\t\t\tmag\t\t");
			file.close();
			Serial.print(F("done: "));
			Serial.println(Fname);
		}
		else Serial.println("error opening file file");
		return true;
	}
}

long SDCardHelper::getTimeLapse()
{
	return lastReadT - lastWorkT;
}

SDCardHelper::operator bool()
{
	//thisReadT = micros();
	file = SD.open(Fname, FILE_WRITE);
	//if (file)
	//{
	//	lastWorkT = thisReadT;
	//}
	//else
	//{
	//	if (getTimeLapse() > recalibrateT)
	//	{
	//		if (!begin()) return false;
	//		file = SD.open(Fname, FILE_WRITE);
	//		Serial.println("Relacibrado SD");
	//	}
	//}
	//lastReadT = thisReadT;
	return file;
}

void SDCardHelper::close()
{
	file.close();
}

size_t SDCardHelper::printNumber(unsigned long n, uint8_t base)
{
	char buf[8 * sizeof(long) + 1]; // Assumes 8-bit chars plus zero byte.
	char *str = &buf[sizeof(buf) - 1];

	*str = '\0';

	// prevent crash if called with base == 1
	if (base < 2) base = 10;

	do {
		char c = n % base;
		n /= base;

		*--str = c < 10 ? c + '0' : c + 'A' - 10;
	} while (n);

	return file.write(str);
}
size_t SDCardHelper::printFloat(double number, uint8_t digits)
{
	size_t n = 0;

	if (isnan(number)) return print("nan");
	if (isinf(number)) return print("inf");
	if (number > 4294967040.0) return print("ovf");  // constant determined empirically
	if (number < -4294967040.0) return print("ovf");  // constant determined empirically

													  // Handle negative numbers
	if (number < 0.0)
	{
		n += print('-');
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i = 0; i < digits; ++i)
		rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	n += print(int_part);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) {
		n += print(".");
	}

	// Extract digits from the remainder one at a time
	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = int(remainder);
		n += print(toPrint);
		remainder -= toPrint;
	}

	return n;
}

size_t SDCardHelper::print(const __FlashStringHelper *ifsh)
{
	return file.print(ifsh);
}
size_t SDCardHelper::print(const String &s)
{
	return file.write(s.c_str(), s.length());
}
size_t SDCardHelper::print(const char str[])
{
	return file.write(str);
}
size_t SDCardHelper::print(char c)
{
	return file.write(c);
}
size_t SDCardHelper::print(unsigned char b, int base)
{
	return print((unsigned long)b, base);
}
size_t SDCardHelper::print(int n, int base)
{
	return print((long)n, base);
}
size_t SDCardHelper::print(unsigned int n, int base)
{
	return print((unsigned long)n, base);
}
size_t SDCardHelper::print(long n, int base)
{
	if (base == 0) {
		return file.write(n);
	}
	else if (base == 10) {
		if (n < 0) {
			int t = print('-');
			n = -n;
			return printNumber(n, 10) + t;
		}
		return printNumber(n, 10);
	}
	else {
		return printNumber(n, base);
	}
}
size_t SDCardHelper::print(unsigned long n, int base)
{
	if (base == 0) return file.write(n);
	else return printNumber(n, base);
}
size_t SDCardHelper::print(double n, int digits)
{
	return printFloat(n, digits);
}
size_t SDCardHelper::print(const Printable& x)
{
	return file.print(x);
}

size_t SDCardHelper::println(void)
{
	return file.write("\r\n");
}
size_t SDCardHelper::println(const __FlashStringHelper *ifsh)
{
	size_t n = print(ifsh);
	n += println();
	return n;
}
size_t SDCardHelper::println(const String &s)
{
	size_t n = print(s);
	n += println();
	return n;
}
size_t SDCardHelper::println(const char c[])
{
	size_t n = print(c);
	n += println();
	return n;
}
size_t SDCardHelper::println(char c)
{
	size_t n = print(c);
	n += println();
	return n;
}
size_t SDCardHelper::println(unsigned char b, int base)
{
	size_t n = print(b, base);
	n += println();
	return n;
}
size_t SDCardHelper::println(int num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}
size_t SDCardHelper::println(unsigned int num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}
size_t SDCardHelper::println(long num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}
size_t SDCardHelper::println(unsigned long num, int base)
{
	size_t n = print(num, base);
	n += println();
	return n;
}
size_t SDCardHelper::println(double num, int digits)
{
	size_t n = print(num, digits);
	n += println();
	return n;
}
size_t SDCardHelper::println(const Printable& x)
{
	size_t n = print(x);
	n += println();
	return n;
}

size_t SDCardHelper::printab(void)
{
	return file.write("\t");
}
size_t SDCardHelper::printab(const __FlashStringHelper *ifsh)
{
	size_t n = print(ifsh);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(const String &s)
{
	size_t n = print(s);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(const char c[])
{
	size_t n = print(c);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(char c)
{
	size_t n = print(c);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(unsigned char b, int base)
{
	size_t n = print(b, base);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(int num, int base)
{
	size_t n = print(num, base);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(unsigned int num, int base)
{
	size_t n = print(num, base);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(long num, int base)
{
	size_t n = print(num, base);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(unsigned long num, int base)
{
	size_t n = print(num, base);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(double num, int digits)
{
	size_t n = print(num, digits);
	n += printab();
	return n;
}
size_t SDCardHelper::printab(const Printable& x)
{
	size_t n = print(x);
	n += printab();
	return n;
}

