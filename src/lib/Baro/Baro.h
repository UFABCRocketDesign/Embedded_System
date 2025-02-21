// Baro.h

#pragma once

#ifndef _BARO_h
#define _BARO_h

#include <Wire.h>
#include "../Sens/Sens.h"

class Baro : public Sens
{
protected:
	float celcius;
	float pascal;

public:
	Baro(uint8_t address, float recalT = 0.1);
	float getTemperature();
	float getPressure();
};

#endif