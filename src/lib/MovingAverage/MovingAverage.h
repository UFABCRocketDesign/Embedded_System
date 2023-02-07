// MovingAverage.h

#pragma once

#ifndef _MOVINGAVERAGE_h
#define _MOVINGAVERAGE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "../RoundArray/RoundArray.h"

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

#endif
