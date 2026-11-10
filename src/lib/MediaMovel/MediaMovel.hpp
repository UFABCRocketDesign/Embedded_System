// MediaMovel.hpp

#pragma once

#ifndef _MEDIAMOVEL_HPP
#define _MEDIAMOVEL_HPP

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class MediaMovel
{
	const unsigned int N;
	float *Vals = new float[N];
	float media, max = 0, min, sigma;

public:
	MediaMovel(int n); // Inicializadore
	~MediaMovel();
	float addValor(const float &valor);
	float getMedia();
	float getVar();
	float getMax();
	float getMin();
	operator float();
};

#endif