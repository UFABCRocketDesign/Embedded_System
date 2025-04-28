// Apogeu.h

#pragma once

#ifndef _APOGEU_h
#define _APOGEU_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef ARDUINO_AVR_MEGA2560
	#include "../SerialFilter/SerialFilter.h"
#else
	#include "../MovingAverage/MovingAverage.h"
#endif // ARDUINO_AVR_MEGA2560

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