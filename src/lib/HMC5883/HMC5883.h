// HMC5883.h

#pragma once

#ifndef _HMC5883_h
#define _HMC5883_h

#include "../Magn/Magn.h"

class HMC5883 : public Magn
{
public:
	HMC5883(long recalT = 0.1);
	void begin();
	bool readAll();
};

#endif