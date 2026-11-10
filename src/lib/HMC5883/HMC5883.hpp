// HMC5883.hpp

#pragma once

#ifndef _HMC5883_HPP
#define _HMC5883_HPP

#include "../Magn/Magn.hpp"

class HMC5883 : public Magn
{
public:
	HMC5883(float recalT = 0.1f);
	void begin();
	bool readAll();
};

#endif