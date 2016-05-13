// Classes.h

#ifndef _CLASSES_h
#define _CLASSES_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>
class Baro  //BMP085
{
public:	//Inicializadores
	Baro();
	Baro(int ad);
private:
	const int BMP085_Address = 0x77;
	const int OSS = 0;	//Oversampling Setting
	int ac1, ac2, ac3;	//Calibration values
	unsigned int ac4, ac5, ac6;
	int b1, b2;
	int mb, mc, md;
	long b5;
	float celcius, meters, base = 0;//Bmp
	long pascal;
	char ReadChar(unsigned char address);
	int ReadInt(unsigned char address);
public:
	void begin();
	float readTemperature();
	float getTemperature();
	long readPressure();
	long getPressure();
	float readAltitude(int Base);
	float readAltitude();
	float getAltitude();
	void readAll();
	float readZero(int i);
	float getZero();
};

class MediaMovel
{
public:	//Inicializadores
	MediaMovel(int n);
private:
	const unsigned int N;
	float* Vals = new float[N];
	float media, max = 0, min;
public:
	float addValor(float valor);
	float getMedia();
	float getMax();
	float getMin();
};

class Apogeu
{
public:
	Apogeu(int n, int r, long s);
private:
	boolean apgExterno = 0;
	long  apgTm = 0;
	float apgPt = 0;
	long TempMax;
	const unsigned int N, R, S;
	bool *temp = new bool[R];
	float* Alt = new float[N];
	float* altMed = new float[N];
public:
	void resetTimer();
	float addAltitude(float H);
	float getApgPt();
	float getApgTm();
	boolean getApogeu();
	boolean apgAlpha();
	boolean apgBeta();
	boolean apgGamma();
	boolean apgPi();
	boolean apgOmega();
	void setOmega(boolean apgE);
	int serial();
};

class Mag //HMC5883
{
public: //Inicializadores
	Mag();
	Mag(int ad);
private:
	const uint8_t HMC5883_Address = 0x1E;
	const int Mag_Xmsb = 0x03;
	float X, Y, Z;
public:
	void begin();
	void readAll();
	float getX();
	float getY();
	float getZ();
};

class Giro  //L3G4200D
{
public: //Inicializadores
	Giro(int sc);
	Giro(int sc, int ad);
private:
	const uint8_t L3G4200D_Address = 105;
	const uint8_t scale;
	const uint8_t Gyro_Xlsb = 0x28;
	const uint8_t Gyro_Xmsb = 0x29;
	const uint8_t Gyro_Ylsb = 0x2A;
	const uint8_t Gyro_Ymsb = 0x2B;
	const uint8_t Gyro_Zlsb = 0x2C;
	const uint8_t Gyro_Zmsb = 0x2D;
	const uint8_t CTRL_REG1 = 0x20;
	const uint8_t CTRL_REG2 = 0x21;
	const uint8_t CTRL_REG3 = 0x22;
	const uint8_t CTRL_REG4 = 0x23;
	const uint8_t CTRL_REG5 = 0x24;
	float X, Y, Z;
public:
	void begin();
	void readAll();
	float getX();
	float getY();
	float getZ();
};


#endif

