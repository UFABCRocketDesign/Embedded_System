// VirtualGyro.hpp

#pragma once

#ifndef _VIRTUALGYRO_HPP
#define _VIRTUALGYRO_HPP

#include "../Gyro/Gyro.hpp"
#include "../VirtualBridge/VirtualBridge.hpp"

class VirtualGyro : public Gyro
{
	VirtualBridge &br;
	int8_t   hX, hY, hZ;
	uint16_t lastSeq;

public:
	VirtualGyro(VirtualBridge &bridge = Bridge, float recalT = 0.1f);
	void begin() override;
	bool readAll() override;
};

#endif // _VIRTUALGYRO_H