// SerialFilter.h

#pragma once

#ifndef _SERIALFILTER_h
#define _SERIALFILTER_h

#include "../MovingAverage/MovingAverage.h"

class SerialFilter
{
	const int N;
	MovingAverage **MM = new MovingAverage *[N];

public:
	SerialFilter(int n, int v);
	SerialFilter(int n, int V[]);
	float addValor(const float &valor);
	float getMedia();
	float operator=(const float &valor);
	operator float();
};

#endif