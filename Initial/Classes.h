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


#endif

