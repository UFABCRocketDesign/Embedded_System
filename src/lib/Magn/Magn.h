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
	Magn(long recalT = 0.1);
	void begin();
	bool readAll();
};

#endif