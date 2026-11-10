// MovingAverage.hpp

#pragma once

#ifndef _MOVINGAVERAGE_HPP
#define _MOVINGAVERAGE_HPP

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../RoundArray/RoundArray.hpp"

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
