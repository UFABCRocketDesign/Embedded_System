// ComProtocol.h

#pragma once

#ifndef _COMPROTOCOL_h
#define _COMPROTOCOL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class ComProtocol
{
public:
	enum information
	{
		none,
		apgDeteced, // Apogee Detected
		apgHeightV, // Apogee height registered value
		apgMomentV, // Apogee moment registered value
		FirstDNorm, // First Deploy Normal
		SecndDNorm, // Second Deploy Normal
		FirstDBack, // First Deploy Backup
		SecndDBack, // Seconf Deploy Backup
		WUFDrctvOf, // Run out of WUF directive
		SystemBoot	// When System turn on
	};
	ComProtocol(HardwareSerial &S, unsigned long baud);

protected:
	HardwareSerial &Com;
	const unsigned long baudRate;
	information lastInfo = none;
	long lastInteger = 0;
	float lastFloating = 0.0f;
	bool successful = false;

public:
	void begin();
	void inform(information info);
	void informInteger(information info, long rawData = 0);
	void informFloating(information info, float rawData = 0.0f);
	information understand();
	long receiveInteger();
	float receiveFloating();
};

#endif
