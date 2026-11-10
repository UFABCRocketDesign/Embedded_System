// VirtualBaro.hpp

#pragma once

#ifndef _VIRTUALBARO_HPP
#define _VIRTUALBARO_HPP

#include "../Baro/Baro.hpp"
#include "../VirtualBridge/VirtualBridge.hpp"

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