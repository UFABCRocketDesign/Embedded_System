// ADXL345.h

#pragma once

#ifndef _ADXL345_h
#define _ADXL345_h

#include "../Accel/Accel.h"

class ADXL345 : public Accel
{
public:
ADXL345(float recalT = 0.1f);
	void begin();
	bool readAll();
};

#endif