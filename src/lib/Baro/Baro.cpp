#include "Baro.h"

///Barometro
Baro::Baro(uint8_t address, float recalT) :Sens(address, recalT)
{
}

float Baro::getTemperature()
{
	return celcius;
}

float Baro::getPressure()
{
	return pascal;
}