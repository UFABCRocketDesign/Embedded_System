// ADXL345.hpp

#pragma once

#ifndef _ADXL345_HPP
#define _ADXL345_HPP

#include "../Accel/Accel.hpp"

class ADXL345 : public Accel
{
public:
ADXL345(float recalT = 0.1f);
	void begin();
	bool readAll();
};

#endif