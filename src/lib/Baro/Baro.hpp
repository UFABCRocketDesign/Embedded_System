// Baro.hpp

#pragma once

#ifndef _BARO_HPP
#define _BARO_HPP

#include <Wire.h>
#include "../Sens/Sens.hpp"

class Baro : public Sens
{
protected:
	float celcius;
	float pascal;

public:
	Baro(uint8_t address, float recalT = 0.1f);
	float getTemperature();
	float getPressure();
};

#endif