// VirtualMagn.h

#pragma once

#ifndef _VIRTUALMAGN_H
#define _VIRTUALMAGN_H

#include "../Magn/Magn.h"
#include "../VirtualBridge/VirtualBridge.h"

class VirtualMagn : public Magn
{
	VirtualBridge &br;
	int8_t   hX, hY, hZ;
	uint16_t lastSeq;

public:
	VirtualMagn(VirtualBridge &bridge = Bridge, float recalT = 0.1f);
	void begin() override;
	bool readAll() override;
};

#endif // _VIRTUALMAGN_H