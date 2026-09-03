// GNSS.h

#pragma once

#ifndef _GNSS_h
#define _GNSS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../Sens/Sens.h"
#include "../Helpful/Helpful.h"

class GNSS : public Sens
{
protected:
	unsigned char satellites = 0;
	unsigned long precision = 0;
	unsigned long chars = 0;
	unsigned long age = 0;
	float latitude = 0;
	float longitude = 0;
	float altitude = 0;
	float Kph = 0;
	float mps = 0;
	int year = 0;
	byte month = 0;
	byte day = 0;
	byte hour = 0;
	byte minute = 0;
	byte second = 0;
	byte GMT = 0;

public:
	GNSS(uint8_t address = 0, float recalT = 0.1f);

	Helpful util;	// Declaration of helpful object to GNSS

	unsigned char getSatellites();
	unsigned long getPrecision();
	unsigned long getChars();
	unsigned long getAge();
	float getLatitude();
	float getLongitude();
	float getAltitude();
	float getKph();
	float getMps();
	int getYear();
	byte getMonth();
	byte getDay();
	byte getHour(bool gmt = 1);
	byte getMinute();
	byte getSecond();
	byte getGMT();
	void setGMT(byte h);
	bool isNew();
};

#endif // _GNSS_h
