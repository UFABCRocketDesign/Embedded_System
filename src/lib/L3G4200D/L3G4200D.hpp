// L3G4200D.hpp

#pragma once

#ifndef _L3G4200D_HPP
#define _L3G4200D_HPP

#include "../Gyro/Gyro.hpp"

class L3G4200D : public Gyro
{
public:
	L3G4200D(long sc, float recalT = 0.1f);
	void begin();
	bool readAll();
};

#endif