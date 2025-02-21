// Accel.h

#pragma once

#ifndef _ACCEL_h
#define _ACCEL_h

#include <Wire.h>
#include "../Sens/Sens.h"
#include "../TriA/TriA.h"

class Accel : public Sens, public TriA
{
public:
	Accel(uint8_t address, long recalT = 0.1);
};

#endif