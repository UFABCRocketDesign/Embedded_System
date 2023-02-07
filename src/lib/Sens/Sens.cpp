#include "Sens.h"

/// Sensor base class
Sens::Sens(uint8_t add, long recalT) : address(add), recalibrateT((long)(recalT * 1000000))
{
}
long Sens::getTimeLapse()
{
	return lastReadT - lastWorkT;
}
Sens::operator bool()
{
	return readAll();
}