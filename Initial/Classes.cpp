#include "Classes.h"

///Barômetro
Baro::Baro()
{
}
void Baro::begin()
{
	Wire.begin();
	Serial.begin(250000);
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
char Baro::ReadChar(unsigned char address)
{
	Wire.beginTransmission(BMP085_Address);
	Wire.write(address);
	Wire.endTransmission();

	Wire.requestFrom(BMP085_Address, 1);
	unsigned long temp = micros();
	while (!Wire.available())
	{
		if (temp + 100000 < micros())break;
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
		if (temp + 100000 < micros())break;
	}
	msb = Wire.read();
	lsb = Wire.read();

	return (int)msb << 8 | lsb;
}
float Baro::readTemperature()
{
	unsigned int ut;

	// Write 0x2E into Register 0xF4
	// This requests a temperature reading
	Wire.beginTransmission(BMP085_Address);
	Wire.write(0xF4);
	Wire.write(0x2E);
	Wire.endTransmission();

	// Wait at least 4.5ms
	delay(5);

	// Read two bytes from registers 0xF6 and 0xF7
	ut = ReadInt(0xF6);

	b5 = ((((long)ut - (long)ac6)*(long)ac5) >> 15) + ((long)mc << 11) / (((((long)ut - (long)ac6)*(long)ac5) >> 15) + md);
	celcius = (float)((b5 + 8) >> 4) / 10;
	return celcius;
}
float Baro::getTemperature()
{
	return celcius;
}
long Baro::readPressure() {
	long x1, x2, x3, b3, b6, p;
	unsigned long b4, b7;
	unsigned char msb, lsb, xlsb;
	unsigned long up = 0;

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
long Baro::getPressure()
{
	return pascal;
}
float Baro::readAltitude(int Base)
{
	meters = ((1 - pow((float)pascal / 101325, 1 / 5.25588)) / 0.0000225577) - Base;
	return meters;
}
float Baro::readAltitude()
{
	meters = ((1 - pow((float)pascal / 101325, 1 / 5.25588)) / 0.0000225577) - base;
	return meters;
}
float Baro::getAltitude()
{
	return meters;
}
void Baro::readAll()
{
	///Faz a leitura de todos os eixos///
	readTemperature();
	readPressure();
	readAltitude();
}
float Baro::readZero(unsigned int I)
{
	float Regis = 0;
	for (byte i = 0; i < I; i++)
	{
		readAll();
		Regis += meters;
	}
	base = Regis/I;
	return base;
}
float Baro::getZero()
{
	return base;
}


///Filtro de m�dia m�vel
MediaMovel::MediaMovel(int n) : N(n)
{

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


///Rotinas de verifica��o de apogeu
Apogeu::Apogeu(int n, int r, long s) : N(n), R(r), S(s)
{
}
void Apogeu::resetTimer()
{
	TempMax = micros();
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
	for (uint8_t i = R - 1; i > 0; i--)
	{
		altMed[i] = altMed[i - 1];
	}
	altMed[0] = Sum / N;
	if (altMed[0] > apgPt)
	{
		apgPt = Alt[0];
		apgTm = micros() - TempMax;
	}
	return altMed[0];
}
float Apogeu::getAltutude()
{
	return altMed[0];
}
float Apogeu::getApgPt()
{
	return apgPt;
}
float Apogeu::getApgTm()
{
	return apgTm;
}
boolean Apogeu::getApogeu()
{
	boolean *temp = new boolean[R];
	for (uint8_t i = 0; i < R; i++) temp[i] = 1;
	for (uint8_t i = R - 1; i > 0; i--)
	{
		for (uint8_t j = R - 1; j > 0; j = j - R - i)
		{
			if (altMed[j] > altMed[j - 1]) temp[i] &= 1;
			else temp[i] &= 0;
		}
	}
	return false;
}
boolean Apogeu::apgAlpha()
{
	volatile boolean temp = 1;
	for (byte i = R - 1; i > 0; i--)
	{
		if (altMed[i] > altMed[i - 1]) temp &= 1;
		else temp &= 0;
	}
	return temp;
}
boolean Apogeu::apgBeta()
{
	volatile boolean temp = 1;
	for (byte i = R - 1; i > 0; i -= 2)
	{
		if (altMed[i] > altMed[i - 1]) temp &= 1;
		else temp &= 0;
	}
	return temp;
}
boolean Apogeu::apgGamma()
{
	volatile boolean temp = 1;
	for (byte i = R - 1; i > 0; i -= 3)
	{
		if (altMed[i] > altMed[i - 1]) temp &= 1;
		else temp &= 0;
	}
	return temp;
}
boolean Apogeu::apgPi()
{
	return (altMed[0] < (altMed[R - 1] + S));
}
boolean Apogeu::apgOmega()
{
	return apgExterno;
}
void Apogeu::setOmega(boolean apgE)
{
	apgExterno = apgE;
}
int Apogeu::serial()
{
	for (unsigned int i = 0; i < R; i++)
	{
		temp[i] = 1;
	}
	for (int i = R - 1; i > 0; i--)
	{
		for (int j = R - 1; j > 1; j -= (R - i))
		{
			int all = j - R + i;
			if (all < 0) continue;
			temp[i] &= (altMed[j] < altMed[all]);
		}
	}
	int sum = 0;
	for (unsigned int k = R - 1; k > 0; k--)
	{
		sum += temp[k];
	}
	return sum;
}


///Magnet?metro
Mag::Mag()
{
	
}
void Mag::begin()
{
	Wire.beginTransmission(HMC5883_Address);
	Wire.write(0x02);                 // Seleciona o modo
	Wire.write(0x00);                 // Modo de medicao continuo
	Wire.endTransmission();
}
void Mag::readAll()
{
	///Faz a leitura de todos os eixos///
	Wire.beginTransmission(HMC5883_Address);
	Wire.write(Mag_Xmsb);
	Wire.endTransmission();
	Wire.requestFrom(HMC5883_Address, (uint8_t)6);
	if (6 <= Wire.available())
	{
		X = Wire.read() << 8; //X msb
		X += Wire.read();    //X lsb
		Z = Wire.read() << 8; //Z msb
		Z += Wire.read();    //Z lsb
		Y = Wire.read() << 8; //Y msb
		Y += Wire.read();    //Y lsb
	}
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


///Girosc?pio
Giro::Giro(int sc): scale(sc)
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
void Giro::readAll()
{
	///Faz a leitura de todos os eixos///
	Wire.beginTransmission(L3G4200D_Address);
	Wire.write(Gyro_Xmsb);
	Wire.endTransmission();
	Wire.requestFrom(L3G4200D_Address, (uint8_t)1);
	if (Wire.available())
	{
		X = Wire.read() << 8; //X msb
	}
	Wire.beginTransmission(L3G4200D_Address);
	Wire.write(Gyro_Xlsb);
	Wire.endTransmission();
	Wire.requestFrom(L3G4200D_Address, (uint8_t)1);
	if (Wire.available())
	{
		X += Wire.read();   //X lsb
	}
	Wire.beginTransmission(L3G4200D_Address);
	Wire.write(Gyro_Ymsb);
	Wire.endTransmission();
	Wire.requestFrom(L3G4200D_Address, (uint8_t)1);
	if (Wire.available())
	{
		Y = Wire.read() << 8; //Y msb
	}
	Wire.beginTransmission(L3G4200D_Address);
	Wire.write(Gyro_Ylsb);
	Wire.endTransmission();
	Wire.requestFrom(L3G4200D_Address, (uint8_t)1);
	if (Wire.available())
	{
		Y += Wire.read();   //Y lsb
	}
	Wire.beginTransmission(L3G4200D_Address);
	Wire.write(Gyro_Zmsb);
	Wire.endTransmission();
	Wire.requestFrom(L3G4200D_Address, (uint8_t)1);
	if (Wire.available())
	{
		Z = Wire.read() << 8; //X msb
	}
	Wire.beginTransmission(L3G4200D_Address);
	Wire.write(Gyro_Zlsb);
	Wire.endTransmission();
	Wire.requestFrom(L3G4200D_Address, (uint8_t)1);
	if (Wire.available())
	{
		Z += Wire.read();   //X lsb
	}
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


///Aceler?metro
Acel::Acel()
{
}
void Acel::begin()
{
	Wire.begin();
	Wire.beginTransmission(ADXL345_Address);// enable to measute g data
	Wire.write(Register_2D);
	Wire.write(8);                    //measuring enable
	Wire.endTransmission();           // stop transmitting
}
void Acel::readAll()
{
	///Faz a leitura de todos os eixos///
	Wire.beginTransmission(ADXL345_Address);
	Wire.write(Acel_Xlsb);
	Wire.endTransmission();
	Wire.requestFrom(ADXL345_Address, (uint8_t)6);
	if (6 <= Wire.available())
	{
		X = Wire.read();    //X lsb
		X += Wire.read() << 8;  //X msb
		Y = Wire.read();    //Y lsb
		Y += Wire.read() << 8;  //Y msb
		Z = Wire.read();    //Z lsb
		Z += Wire.read() << 8;  //Z msb
	}
	X /= 256.0;
	Y /= 256.0;
	Z /= 256.0;
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


///Controle de paraquedas
DuDeploy::DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay) : P1(paraPin1), P2(paraPin2), I1(infPin1), I2(infPin2),Tign((long)(ignT * 1000000)), Delay((long)(delay * 1000000))
{
	pinMode(P1, OUTPUT);
	pinMode(P2, OUTPUT);
	pinMode(I1, INPUT_PULLUP);
	pinMode(I2, INPUT_PULLUP);
}
void DuDeploy::resetTimers()
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
	resetTimers();
	return info1() || info2();
}
void DuDeploy::setTmax(float Time)
{
	Tmax =(unsigned long)(Time*1000000);
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
bool DuDeploy::getP1S()
{
	return P1S;
}
bool DuDeploy::getP2S()
{
	return P2S;
}
bool DuDeploy::refresh(float height)
{
	Sys = (P1T_A && P2T_A && (P1S || P2S)) || (P2T_A && !P1S && (!P1T_A || P2S)) || (P2T_A && !P2S && (P1S || !P2T_A));
	Tnow = micros() - TimeZero;
	if (TmaxAux && Tnow > Tmax && !apogee) apogee = true;	//Caso o tempo máximo seja exigido e tenha sido ultrapassado
	if (apogee && (Sys || (!P1S && !P2S && (!P1T_A || !P1T_A))))	//Se o apogeu foi dado
	{
		if (!P1H_A || (P1H_A && height <= P1H))	//Caso altura seja exigida, será verificada
		{
			if (!P1T_A)	//Salvar tempo de P1 somente uma vez
			{
				P1T_A = 1;
				P1T = Tnow;
			}
			if (Tnow < P1T + Tign && P1T_A) P1S = 1;	//Comando efetivo
			else P1S = 0;
		}
		if ((!P2H_A && Tnow > P1T + Delay) || (P2H_A && height <= P2H))	//Verifica tempo de atraso ou altura exigida
		{
			if (!P2T_A)	//Salva tempo de P2 somente uma vez
			{
				P2T_A = 1;
				P2T = Tnow;
			}
			if (Tnow < P2T + Tign && P2T_A) P2S = 1;	//Comando efetivo
			else P2S = 0;
		}
	}
	pinMode(P1, P1S);
	pinMode(P2, P2S);
	return Sys;	//Retornará 1 quando o processo estiver terminado
}
bool DuDeploy::getSysState(bool type=1)
{
	return Sys || (!type && !P1S && !P2S && (!P1T_A || !P1T_A));
}
