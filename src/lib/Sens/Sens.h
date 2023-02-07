// Sens.h

#pragma once

#ifndef _SENS_h
#define _SENS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

/// Sensor base class
class Sens
{
protected:
	const uint8_t address;
	const long recalibrateT;
	unsigned long thisReadT = 0;
	unsigned long lastReadT = 0;
	unsigned long lastWorkT = 0;
	bool state = 0;

public:
	Sens(uint8_t add, long recalT = 0.1);
	virtual void begin() = 0;
	virtual bool readAll() = 0;
	long getTimeLapse();
	operator bool();
};

#endif