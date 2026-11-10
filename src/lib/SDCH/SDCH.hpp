// SDCH.hpp

#pragma once

#ifndef _SDCH_HPP
#define _SDCH_HPP

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include <SD.h>

#include "../Helpful/Helpful.hpp"

class SDCH
{
	const uint8_t CS;
	const String Fname0;
	const String Ftype;
	const uint8_t coef;
	const unsigned long nMax;
	SPIClass &SPI_SD;
	unsigned long number = 0;
	String Fname;
	void newName();
	static unsigned long pow10ul(uint8_t n);
public:
	SDCH(uint8_t csPin, String name, String type = "txt", SPIClass &spi_sd = SPI);
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