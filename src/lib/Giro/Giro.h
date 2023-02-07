// Giro.h

#pragma once

#ifndef _GIRO_h
#define _GIRO_h

#include <Wire.h>
#include "../Sens/Sens.h"
#include "../TriA/TriA.h"

class Giro : public Sens, public TriA
{
public:
	Giro(long sc, long recalT = 0.1);
	void begin();
	bool readAll();

private:
	const long scale;
};

#endif