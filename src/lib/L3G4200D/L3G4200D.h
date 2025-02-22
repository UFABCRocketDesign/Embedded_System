// L3G4200D.h

#pragma once

#ifndef _L3G4200D_h
#define _L3G4200D_h

#include "../Gyro/Gyro.h"

class L3G4200D : public Gyro
{
public:
	L3G4200D(long sc, long recalT = 0.1);
	void begin();
	bool readAll();
};

#endif