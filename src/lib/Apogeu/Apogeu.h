// Apogeu.h

#pragma once

#ifndef _APOGEU_h
#define _APOGEU_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef ARDUINO_AVR_MEGA2560
	#include "../SerialFilter/SerialFilter.h"
#else
	#include "../MovingAverage/MovingAverage.h"
#endif // ARDUINO_AVR_MEGA2560

#include <EEPROM.h>

#define _GERAL_TIME uint32_t(unsigned(/*S*/ 10*(__TIME__[6] - '0') + (__TIME__[7] - '0')) + 60 * \
                            (unsigned(/*M*/ 10*(__TIME__[3] - '0') + (__TIME__[4] - '0')) + 60 * \
                            (unsigned(/*H*/ 10*(__TIME__[0] - '0') + (__TIME__[1] - '0')) + 24 * \
                            (unsigned(/*D*/ 10*(__DATE__[4] - '0') + (__DATE__[5] - '0')) \
                            ))))

class Apogeu
{
	const unsigned int N, R, Rl1;
	const float S;
	const float Rf;
	const uint16_t eeAddress = _GERAL_TIME % (EEPROM.length()-sizeof(float));
	float base = 0;
	float baseMax = 0;
	float baseMin = 0;
	bool usingFixZero = false;
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
	bool fixZero(float maxRange = 10.0f);
	bool getFixZero();
	uint16_t getEEAddress();
	void resetZero();
	void resetAptPt();
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


#endif