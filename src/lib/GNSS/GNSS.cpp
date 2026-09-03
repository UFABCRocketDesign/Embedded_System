#include "GNSS.h"

///Base dos GNSS
GNSS::GNSS(uint8_t address, float recalT) : Sens(address, recalT)
{}

unsigned char GNSS::getSatellites()
{
	return satellites;
}

unsigned long GNSS::getPrecision()
{
	return precision;
}

unsigned long GNSS::getChars()
{
	return chars;
}

unsigned long GNSS::getAge()
{
	return age;
}

float GNSS::getLatitude()
{
	return latitude;
}

float GNSS::getLongitude()
{
	return longitude;
}

float GNSS::getAltitude()
{
	return altitude;
}

float GNSS::getKph()
{
	return Kph;
}

float GNSS::getMps()
{
	return mps;
}

int  GNSS::getYear()
{
	return year;
}

byte GNSS::getMonth()
{
	return month;
}

byte GNSS::getDay()
{
	return day;
}

byte GNSS::getHour(bool)
{
	return hour;
}	// fuso aplicado no readAll()

byte GNSS::getMinute()
{
	return minute;
}

byte GNSS::getSecond()
{
	return second;
}

void GNSS::setGMT(byte h)
{
	GMT = h;
}

byte GNSS::getGMT()
{
	return GMT;
}

bool GNSS::isNew()
{
	return state;
}	// state vem de Sens

