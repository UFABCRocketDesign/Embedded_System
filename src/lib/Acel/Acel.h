#pragma once

#ifndef _ACEL_h
#define _ACEL_h

#include <Wire.h>
#include "../Sens/Sens.h"
#include "../TriA/TriA.h"

class Acel : public Sens, public TriA
{
public:
	Acel(long recalT = 0.1);
	void begin();
	bool readAll();
};

#endif