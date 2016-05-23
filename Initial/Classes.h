// Classes.h

#ifndef _CLASSES_h
#define _CLASSES_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>


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

class Baro  //BMP085
{
public:	//Inicializadores
	Baro(float Tzero);
	Baro();
private:
	const long recalibrateT;
	const int BMP085_Address = 0x77;
	const int OSS = 0;	//Oversampling Setting

	unsigned int ut = 0;
	unsigned long up = 0;

	int b1, b2;
	long b3, b6, p;
	long b5;
	unsigned long b4, b7;
	long x1, x2, x3;
	unsigned char msb, lsb, xlsb;

	int ac1, ac2, ac3;	//Calibration values
	unsigned int ac4, ac5, ac6;
	int mb, mc, md;

	float celcius, meters, base = 0;//Bmp
	long pascal;
	long lastT = 0;
	bool state = 1;

	char ReadChar(unsigned char address);
	int ReadInt(unsigned char address);
	float readTemperature();
	long readPressure();
	float readAltitude(float sealevelP = 101325);
public:
	void begin();
	long getTimeLapse();
	bool readAll(float sealevelP = 101325);
	float readZero(unsigned int I);
	long readSealevel(float altitude);
	float getZero();
	float getTemperature();
	long getPressure();
	float getAltitude(int Base);
	float getAltitude();
};

class Mag //HMC5883
{
public: //Inicializadores
	Mag(float Tzero);
	Mag();
private:
	const long recalibrateT;
	const uint8_t HMC5883_Address = 0x1E;
	const int Mag_Xmsb = 0x03;
	float X, Y, Z;
	long lastT = 0;
	bool state = 1;
public:
	void begin();
	long getTimeLapse();
	bool readAll();
	float getX();
	float getY();
	float getZ();
};

class Giro  //L3G4200D
{
public: //Inicializadores
	Giro(int sc, float Tzero);
	Giro(int sc);
private:
	const long recalibrateT;
	const uint8_t L3G4200D_Address = 105;
	const long scale;
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
	long lastT = 0;
	bool state = 1;
public:
	void begin();
	long getTimeLapse();
	bool readAll();
	float getX();
	float getY();
	float getZ();
};

class Acel  //ADXL345
{
public:  //Inicializadores
	Acel(float Tzero);
	Acel();
private:
	const long recalibrateT;
	const uint8_t ADXL345_Address = 0x53;
	const uint8_t Acel_Xlsb = 0x32;
	const uint8_t Register_ID = 0;
	const uint8_t Register_2D = 0x2D;
	float X, Y, Z;
	long lastT = 0;
	bool state = 0;
public:
	void begin();
	long getTimeLapse();
	bool readAll();
	float getX();
	float getY();
	float getZ();
};

class Apogeu
{
public:
	Apogeu(unsigned int n, unsigned int r, float s);
private:
	boolean apgExterno = 0;
	long  apgTm = 0;
	float apgPt = 0;
	float maxH = 0;
	float minH = 0;
	long TempMax;
	const unsigned int N, R, Rl1;
	const float S;
	float Rf;
	bool *temp = new bool[Rl1];
	float* Alt = new float[N];
	float* altMed = new float[N];
public:
	void resetTimer();
	float addAltitude(float H);
	float getAltutude();
	float getApgPt();
	float getApgTm();
	float getMaxH();
	float getMinH();
	boolean getApogeu();
	boolean apgAlpha();
	boolean apgPi();
	boolean apgOmega();
	void setOmega(boolean apgE);
	float apgSigma(bool serial = 0);
};

class DuDeploy
{
public:
	DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay);

private:
	const unsigned int P1, P2;	//Pinos de comando
	const unsigned int I1, I2;	//Pinos de verificação
	const unsigned long Tign;	//Tempo de comando ativo
	const unsigned long Delay;	//Tempo de atraso dentre comando ( caso altura não determinada)
	unsigned long P1T = 0, P2T = 0;	//Momento que o ignitor ligou
	unsigned long TimeZero = 0;	//Momento zero
	unsigned long Tmax = 0;	//Tempo máximo para segunrança
	unsigned long Tnow = 0;	//Tempo atual (com zeragem)
	float P1H = 0, P2H = 0;	//Altura de comando
	bool P1S = 0, P2S = 0;	//Estado atual do comando
	bool P1H_A = 0, P2H_A = 0;	//Condicional para comando em altura
	bool apogee = 0;	//Representante interno do apogeu
	bool TmaxAux = 0;	//Condicional para verificar tempo de segurança
	bool P1T_A = 0, P2T_A = 0;	//Condicional para momento do ignitor
	bool Sys=0;
public:
	void resetTimers();
	bool info1();	//Retorna informação sobre estado do ignitor de acionamento 1
	bool info2();	//Retorna informação sobre estado do ignitor de acionamento 2
	bool begin();
	void setTmax(float Time);
	void setP1height(float H);
	void setP2height(float H);
	void sealApogee(bool apg);
	bool getApogee();
	bool getP1S();
	bool getP2S();
	bool refresh(float height);
	bool getSysState(bool type);
};

#endif

