// Magn.h

#pragma once

#ifndef _MAGN_h
#define _MAGN_h

#include <Wire.h>
#include "../Sens/Sens.h"
#include "../TriA/TriA.h"

class Magn : public Sens, public TriA
{
public:
	Magn(uint8_t address, long recalT = 0.1);
};

#endif