// ADXL345.h

#pragma once

#ifndef _ADXL345_h
#define _ADXL345_h

#include "../Accel/Accel.h"

class ADXL345 : public Accel
{
public:
ADXL345(long recalT = 0.1);
	void begin();
	bool readAll();
};

#endif