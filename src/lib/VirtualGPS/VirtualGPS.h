// VirtualGPS.h

#pragma once

#ifndef _VIRTUALGPS_H
#define _VIRTUALGPS_H

#include "../GNSS/GNSS.h"
#include "../VirtualBridge/VirtualBridge.h"

class VirtualGPS : public GNSS
{
	VirtualBridge &br;
	int8_t hLat;
	int8_t hLon;
	int8_t hAlt;
	int8_t hSat;
	int8_t hPrec;
	int8_t hChars;
	int8_t hAge;
	int8_t hKph;
	int8_t hMps;
	int8_t hYear;
	int8_t hMonth;
	int8_t hDay;
	int8_t hHour;
	int8_t hMinute;
	int8_t hSecond;
	uint16_t lastSeq;

public:
	VirtualGPS(VirtualBridge &bridge = Bridge, float recalT = 0.1f);
	void begin() override;
	bool readAll() override;
};

#endif // _VIRTUALGPS_H