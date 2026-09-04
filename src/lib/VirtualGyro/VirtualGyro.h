// VirtualGyro.h

#pragma once

#ifndef _VIRTUALGYRO_H
#define _VIRTUALGYRO_H

#include "../Gyro/Gyro.h"
#include "../VirtualBridge/VirtualBridge.h"

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