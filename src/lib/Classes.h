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

#include "Sens/Sens.h"
#include "TriA/TriA.h"
#include "Baro/Baro.h"

#define SERVO_MODE 1
#if SERVO_MODE
#include <Servo.h>
#endif // SERVO_MODE

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
	type* Vals = new type[Len]();
	size_t pointer = 0;
public:
	RoundArray(const unsigned int &len) :Len(len)
	{
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
	float addValor(const float &valor);
	float getMedia();
	float operator=(const float &valor);
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
	HardwareSerial &GpSerial;
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
	GyGPS(HardwareSerial &S, short gmt = 0);
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
	const float Rf;
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
	bool* cond = new bool[Rl1]();
	//float* Alt = new float[N];
	float* altMed = new float[R]();


#ifdef ARDUINO_AVR_MEGA2560
	SerialFilter Alt;
#else
	MovingAverage Alt;
#endif // ARDUINO_AVR_MEGA2560

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
	const unsigned long Delay;	//Tempo de atraso dentre comando (caso altura nao determinada)
	unsigned long P1T = 0, P2T = 0;	//Momento que o ignitor ligou
	unsigned long TimeZero = 0;	//Momento zero
	unsigned long Tmax;	//Tempo maximo para segunranca
	unsigned long Tseal;	//Tempo onde o selo aconteceu
	unsigned long Tdelay;	//Tempo de atraso configurado
	unsigned long P1M, P2M;	//Tempo onde a condicional do paraquedas foi atingida
	unsigned long Tnow = 0;	//Tempo atual (com zeragem)
	float P1H = 0, P2H = 0;	//Altura de comando
	bool P1S = 0, P2S = 0;	//Estado do acionamento
	bool P1S_A = 0, P2S_A = 0;	//Estado atual do comando
	bool P1H_A = 0, P2H_A = 0;	//Condicional para comando em altura
	bool apogee = 0;	//Representante interno do apogeu
	bool TmaxAux;	//Condicional para verificar tempo de seguranca
	bool TdelayAux = 0;	//Condicional para verificar modo de atraso
	bool P1M_A = 0, P2M_A = 0;	//Condicional do paraquedas foi atingida
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
	void setTdelay(float Time);
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

class MonoDeploy
{
	static const bool command = HIGH;	//Active state
	static bool apogee;
	static unsigned long TimeZero;	//Zero reference for timer
	static unsigned long Tseal;	//Seal momment time
	static float height;

	const unsigned long Tign;	//Active time
	const unsigned int cPin;	//Command pin
	const unsigned int iPin;	//Info pin
	const unsigned int sysMode;
#if SERVO_MODE
	Servo motor;
#endif
	unsigned long Tnow;
	unsigned long Tcmd;
	unsigned long Theight;
	unsigned long Tmax;
	float cmdHeight;
	float cmdDelay;
	bool useH = false;	//Command height based
	bool useT = false;	//Command time based
	bool useM = false;
	bool useH_A = false;
	bool useT_A = false;
	bool cmdSeal = false;	//
	bool stateAux = false;
	bool globalState = true;
	bool globalStateAux = false;
	bool sPin = !command;	//Output sate
public:
	MonoDeploy(unsigned int commandPin, unsigned int infoPin, unsigned long actionTime = 5, unsigned int systemMode = 0);

	static void resetTimer();
	static void sealApogee(bool apg);
	static bool getApogee();
	static void putHeight(float H);

	void setHeightCmd(float H);
	void setDelayCmd(float T);
	void setTmax(float T);
	bool begin();
	bool info();
	bool getState(bool type = 1);
	bool getGlobalState();
	void refresh();
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
/*
class ComProtocol
{
public:
	enum information
	{
		none,
		apgDeteced,	//Apogee Detected
		apgHeightV, //Apogee height registered value
		apgMomentV, //Apogee moment registered value
		FirstDNorm,	//First Deploy Normal
		SecndDNorm,	//Second Deploy Normal
		FirstDBack,	//First Deploy Backup
		SecndDBack,	//Seconf Deploy Backup
		WUFDrctvOf,	//Run out of WUF directive
		SystemBoot	//When System turn on
	};
	ComProtocol(HardwareSerial &S, unsigned long baud);
protected:
	HardwareSerial &Com;
	const unsigned long baudRate;
	information lastInfo = none;
	long lastInteger = 0;
	float lastFloating = 0.0f;
	bool successful = false;
public:
	void begin();
	void inform(information info);
	void informInteger(information info, long rawData = 0);
	void informFloating(information info, float rawData = 0.0f);
	information understand();
	long receiveInteger();
	float receiveFloating();
};
*/
#endif
