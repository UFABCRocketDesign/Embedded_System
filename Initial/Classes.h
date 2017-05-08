// Classes.h

#pragma once

#ifndef _CLASSES_h
#define _CLASSES_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <TinyGPS.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

class Sens
{
protected:
	const uint8_t address;
	const long recalibrateT;
	unsigned long thisReadT = 0;
	unsigned long lastReadT = 0;
	unsigned long lastWorkT = 0;
	bool state = 0;
public:
	Sens(uint8_t add, long recalT = 0.1);
	virtual void begin() = 0;
	virtual bool readAll() = 0;
	long getTimeLapse();
	operator bool();
};

class Baro :public Sens
{
	const int OSS = 0;	//Oversampling Setting
	unsigned int ut = 0;
	unsigned long up = 0;
	unsigned long b4, b7;
	long b1, b2, b3, b5, b6;
	long x1, x2, x3;
	int ac1, ac2, ac3;	//Calibration values
	unsigned int ac4, ac5, ac6;
	int mb, mc, md;

	float celcius;
	long pascal;
public:
	Baro(float recalT = 0.1);
	void begin();
	bool readAll();
	float getTemperature();
	long getPressure();
};

class TriA
{
protected:
	float X = 0;
	float Y = 0;
	float Z = 0;
public:
	virtual bool readAll() = 0;
	float getX();
	float getY();
	float getZ();
};

class Acel : public Sens, public TriA
{
public:
	Acel(long recalT = 0.1);
	void begin();
	bool readAll();
};

class Magn :public Sens, public TriA
{
public:
	Magn(long recalT = 0.1);
	void begin();
	bool readAll();
};

class Giro :public Sens, public TriA
{
public:
	Giro(long sc, long recalT = 0.1);
	void begin();
	bool readAll();
private:
	const long scale;
};

class Helpful
{
	unsigned long conEach = 0;
	unsigned long timerEach = 0;
	unsigned long lapseT = 0;
	unsigned long T0 = 0;
	unsigned long endT = 0;
	bool forTstate = 0;
	bool one = 0;
	unsigned long count = 0;
	float maxi = 0, mini = 0;
public:
	bool mem;
	void begin();
	unsigned long counter();
	unsigned long getCount();
	unsigned long counterReset();
	bool eachN(const unsigned int &N);
	bool eachT(const float &T);
	void forT(const float &T);
	bool forT();
	float lapse();
	float sinceBegin();
	bool oneTime();
	void oneTimeReset();
	void comparer(const float &n);
	float getMax();
	float getMin();
};

template <typename type> class RoundArray
{
	const size_t Len;
	type* Vals = new type[Len];
	size_t pointer = 0;
public:
	RoundArray(const unsigned int &len) :Len(len)
	{
		for (unsigned int i = 0; i < Len; i++) Vals[i] = 0;
	}
	~RoundArray() // Destructor
	{
		delete[] Vals;
	}
	void push(const type &var)
	{
		Vals[pointer] = var;
		pointer = pointer + 1 == Len ? 0 : pointer + 1;
	}
	operator float()
	{
		return Vals[pointer];
	}
	type &operator[](int index)
	{
		return Vals[(Len + pointer - index - 1) % Len];
	}
	void operator=(const type &var)
	{
		push(var);
	}
	size_t length()
	{
		return Len;
	}
};

class MovingAverage
{
	const unsigned int N;
	RoundArray<float> Vals;
	float media = 0, max = 0, min, sigma;
public:
	MovingAverage(int n);
	float addValor(const float &valor);
	float getMedia();
	float getVar();
	float getMax();
	float getMin();
	float operator=(const float &valor);
	operator float();
};

/*
class MediaMovel
{
	const unsigned int N;
	float* Vals = new float[N];
	float media, max = 0, min, sigma;
public:
	MediaMovel(int n);	//Inicializadore
	~MediaMovel();
	float addValor(const float &valor);
	float getMedia();
	float getVar();
	float getMax();
	float getMin();
	operator float();
};*/

class SerialFilter
{
	const int N;
	MovingAverage **MM = new MovingAverage*[N];
public:
	SerialFilter(int n, int v);
	SerialFilter(int n, int V[]);
	float operator=(const float &in);
	operator float();
};

class Term	//LM35
{
const byte Apin;
public:
Term(byte aPin);
float read();
};

class GyGPS : public Sens
{
	//#define GpSerial Serial1
	HardwareSerial GpSerial;
	TinyGPS gps;							//GPS object declaration
	const unsigned short GMT;
	unsigned short sentences = 0;			//GPS variables declaration
	unsigned short failed = 0;
	unsigned char satellites = 0;
	unsigned long precision = 0;
	unsigned long chars = 0;
	unsigned long age = 0;
	float latitude = 0;
	float longitude = 0;
	float altitude = 0;
	float Kph = 0;
	float mps = 0;
	int year = 0;
	byte month = 0;
	byte day = 0;
	byte hour = 0;
	byte minute = 0;
	byte second = 0;
public:
	GyGPS(HardwareSerial S, short gmt = 0);
	Helpful util;						//Declaration of helpful object to GPS
	unsigned short getFailed();				//Return functions
	unsigned short getSentences();
	unsigned char getSatellites();
	unsigned long getPrecision();
	unsigned long getChars();
	unsigned long getAge();
	float getLatitude();
	float getLongitude();
	float getAltitude();
	float getKph();
	float getMps();
	int getYear();
	byte getMonth();
	byte getDay();
	byte getHour(bool gmt = 1);
	byte getMinute();
	byte getSecond();
	void begin();
	bool readAll();							//GPS function declaration
	bool isNew();
};

class Apogeu
{
	const unsigned int N, R, Rl1;
	const float S;
	float Rf;
	float base = 0;
	int baseIndex = 0;

	bool Alpha = 0;
	bool Beta = 0;
	bool Gamma = 0;
	float Sigma = 0;

	long  apgTm = 0;
	float apgPt = 0;
	float maxH = 0;
	float minH = 0;
	long TimeZero = 0;
	bool apg_A = 0;
	bool* cond = new bool[Rl1];
	//float* Alt = new float[N];
	float* altMed = new float[R];
	
	//MovingAverage Alt;
	SerialFilter Alt;
public:
	Apogeu(unsigned int n, unsigned int r, float s);
	float addZero(long P, float sealevelP = 101325);
	float getZero();
	void resetZero();
	void resetTimer();
	//float addAltitude(float H);
	float calcAlt(const long &P, float sealevelP = 101325);
	void setGamma(bool apgE);

	bool apgAlpha(bool serial = 0);
	bool apgBeta();
	bool apgGamma();
	float apgSigma(bool serial = 0);

	bool getApogeu(float percent = 0.9f, bool type = 1);
	bool getAlpha();
	bool getBeta();
	bool getGamma();
	float getSigma();

	float getAltitude();
	float getAltitude(float B);
	float getApgPt();
	float getApgTm();
	float getMaxH();
	float getMinH();
};

class DuDeploy
{
	const unsigned int P1, P2;	//Pinos de comando
	const unsigned int I1, I2;	//Pinos de verificacao
	const unsigned long Tign;	//Tempo de comando ativo
	const unsigned long Delay;	//Tempo de atraso dentre comando ( caso altura nao determinada)
	unsigned long P1T = 0, P2T = 0;	//Momento que o ignitor ligou
	unsigned long TimeZero = 0;	//Momento zero
	unsigned long Tmax;	//Tempo máximo para segunranca
	unsigned long Tnow = 0;	//Tempo atual (com zeragem)
	float P1H = 0, P2H = 0;	//Altura de comando
	bool P1S = 0, P2S = 0;
	bool P1S_A = 0, P2S_A = 0;	//Estado atual do comando
	bool P1H_A = 0, P2H_A = 0;	//Condicional para comando em altura
	bool apogee = 0;	//Representante interno do apogeu
	bool TmaxAux;	//Condicional para verificar tempo de seguranca
	bool P1T_A = 0, P2T_A = 0;	//Condicional para momento do ignitor
	bool P1seal = 0, P2seal = 0;	//Selo de comando
	bool emer = 0, P1H_Am = 0, P2H_Am = 0;
public:
	DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay, float tMax = 0);
	void resetTimer();
	bool info1();	//Retorna informacao sobre estado do ignitor de acionamento 1
	bool info2();//Retorna informacao sobre estado do ignitor de acionamento 2
	bool begin();
	void setTmax(float Time);
	void setP1height(float H);
	void setP2height(float H);
	void sealApogee(bool apg);
	bool getApogee();
	bool getP1S(bool type = 1);
	bool getP2S(bool type = 1);
	bool getSysState(bool type = 1);
	void refresh(float height);
	void emergency(bool state = 1);
};

class SDCH
{
	const uint8_t CS;
	const String Fname0;
	const String Ftype;
	const uint8_t coef;
	const unsigned long nMax;
	unsigned long number = 0;
	String Fname;
	void newName();
public:
	SDCH(uint8_t csPin, String name, String type = "txt");
	File theFile;
	Helpful util;
	bool begin();
	bool open();
	size_t tab();
	void close();
	operator bool();
	String getFname();
};

#endif
