// Helpful.h

#pragma once

#ifndef _HELPFUL_h
#define _HELPFUL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

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

#endif