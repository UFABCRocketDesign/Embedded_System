#pragma once

#ifndef _ACCEL_HPP
#define _ACCEL_HPP

#include <Wire.h>
#include "../Sens/Sens.hpp"
#include "../TriA/TriA.hpp"

class Accel : public Sens, public TriA
{
protected:
	const long scale;

public:
	Accel(long sc, uint8_t address, float recalT = 0.1f);
};

#endif