// Gyro.h

#pragma once

#ifndef _GYRO_h
#define _GYRO_h

#include <Wire.h>
#include "../Sens/Sens.h"
#include "../TriA/TriA.h"

class Gyro : public Sens, public TriA
{
protected:
	const long scale;
public:
	Gyro(long sc, uint8_t address, float recalT = 0.1f);

};

#endif