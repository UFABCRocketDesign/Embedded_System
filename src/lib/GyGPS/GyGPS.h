// GyGPS.h

#pragma once

#ifndef _GYGPS_h
#define _GYGPS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define USE_TINY_PLUS 1

#if USE_TINY_PLUS
#include <TinyGPSPlus.h>
#else
#include <TinyGPS.h>
#endif // USE_TINY_PLUS

#include "../Sens/Sens.h"
#include "../Helpful/Helpful.h"

class GyGPS : public Sens
{
	// #define GpSerial Serial1
	HardwareSerial &GpSerial;
#if USE_TINY_PLUS
	TinyGPSPlus gps; // GPS object declaration
#else
	TinyGPS gps; // GPS object declaration
#endif // USE_TINY_PLUS
	const unsigned short GMT;
	unsigned short sentences = 0; // GPS variables declaration
	unsigned short failed = 0;
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

public:
	GyGPS(HardwareSerial &S, short gmt = 0);
	Helpful util;				// Declaration of helpful object to GPS
	unsigned short getFailed(); // Return functions
	unsigned short getSentences();
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
	void begin();
	bool readAll(); // GPS function declaration
	bool isNew();
};

#endif