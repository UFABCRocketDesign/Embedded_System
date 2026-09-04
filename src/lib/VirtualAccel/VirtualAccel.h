// VirtualAccel.h

#pragma once

#ifndef _VIRTUALACCEL_H
#define _VIRTUALACCEL_H

#include "../Accel/Accel.h"
#include "../VirtualBridge/VirtualBridge.h"

class VirtualAccel : public Accel
{
	VirtualBridge &br;
	int8_t   hX, hY, hZ;
	uint16_t lastSeq;

public:
	VirtualAccel(VirtualBridge &bridge = Bridge, float recalT = 0.1f);
	void begin() override;
	bool readAll() override;
};

#endif // _VIRTUALACCEL_H