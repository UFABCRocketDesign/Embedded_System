// Baro.h

#pragma once

#ifndef _BARO_h
#define _BARO_h

#include <Wire.h>
#include "../Sens/Sens.h"

class Baro : public Sens
{
	const int OSS = 0; // Oversampling Setting
	unsigned int ut = 0;
	unsigned long up = 0;
	unsigned long b4, b7;
	long b1, b2, b3, b5, b6;
	long x1, x2, x3;
	int ac1, ac2, ac3; // Calibration values
	unsigned int ac4, ac5, ac6;
	int mb, mc, md;

	float celcius;
	long pascal;

public:
	Baro(float recalT = 0.1);
	void begin();
	bool readAll();
	float getTemperature();
	long getPressure();
};

#endif