// SDCH.h

#pragma once

#ifndef _SDCH_h
#define _SDCH_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include <SD.h>

#include "../Helpful/Helpful.h"

class SDCH
{
	const uint8_t CS;
	const String Fname0;
	const String Ftype;
	const uint8_t coef;
	const unsigned long nMax;
	unsigned long number = 0;
	String Fname;
	void newName();
public:
	SDCH(uint8_t csPin, String name, String type = "txt");
	File theFile;
	Helpful util;
	bool begin();
	bool open();
	size_t tab();
	void close();
	operator bool();
	String getFname();
};

#endif