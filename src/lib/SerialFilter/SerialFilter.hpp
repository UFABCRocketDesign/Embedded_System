// SerialFilter.hpp

#pragma once

#ifndef _SERIALFILTER_HPP
#define _SERIALFILTER_HPP

#include "../MovingAverage/MovingAverage.hpp" // Media movel simples

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