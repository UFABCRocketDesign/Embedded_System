#include "Gyro.h"

///Giroscopio
Gyro::Gyro(long sc, uint8_t address, float recalT) : scale(sc), Sens(address, recalT)
{
}
