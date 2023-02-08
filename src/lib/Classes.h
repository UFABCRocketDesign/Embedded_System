// Classes.h

#pragma once

#ifndef _CLASSES_h
#define _CLASSES_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
// #include <TinyGPS.h>
// #include <Wire.h>

#define SERVO_MODE 0

#include "Sens/Sens.h"
#include "TriA/TriA.h"
#include "Baro/Baro.h"
#include "Acel/Acel.h"
#include "Magn/Magn.h"
#include "Giro/Giro.h"
#include "Helpful/Helpful.h"
#include "RoundArray/RoundArray.h"
#include "MovingAverage/MovingAverage.h"
#include "SerialFilter/SerialFilter.h"
#include "GyGPS/GyGPS.h"
#include "Apogeu/Apogeu.h"
#include "DuDeploy/DuDeploy.h"
#include "MonoDeploy/MonoDeploy.h"
#include "SDCH/SDCH.h"

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

class Term	//LM35
{
	const byte Apin;
public:
	Term(byte aPin);
	float read();
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
