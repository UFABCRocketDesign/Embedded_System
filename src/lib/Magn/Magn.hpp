// Magn.h

#pragma once

#ifndef _MAGN_h
#define _MAGN_h

#include <Wire.h>
#include "../Sens/Sens.hpp"
#include "../TriA/TriA.hpp"

class Magn : public Sens, public TriA
{
public:
	Magn(uint8_t address, float recalT = 0.1f);
};

#endif