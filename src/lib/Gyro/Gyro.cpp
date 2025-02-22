#include "Gyro.h"

///Giroscopio
Gyro::Gyro(long sc, uint8_t address, long recalT) : scale(sc), Sens(address, recalT)
{
}
