// Term.h

#pragma once

#ifndef _TERM_h
#define _TERM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class Term // LM35
{
	const byte Apin;

public:
	Term(byte aPin);
	float read();
};

#endif
