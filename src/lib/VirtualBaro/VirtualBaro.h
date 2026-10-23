// VirtualBaro.h

#pragma once

#ifndef _VIRTUALBARO_h
#define _VIRTUALBARO_h

#include "../Baro/Baro.h"
#include "../VirtualBridge/VirtualBridge.h"

class VirtualBaro : public Baro
{
	VirtualBridge &br;
	int8_t   hP, hT;
	uint16_t lastSeq;

public:
	VirtualBaro(VirtualBridge &bridge = Bridge, float recalT = 0.1f);
	void begin() override;
	bool readAll() override;
};

#endif // _VIRTUALBARO_h