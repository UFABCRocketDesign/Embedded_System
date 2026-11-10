// Term.hpp

#pragma once

#ifndef _TERM_HPP
#define _TERM_HPP

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
