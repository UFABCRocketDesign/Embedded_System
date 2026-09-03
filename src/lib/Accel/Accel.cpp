#include "Accel.h"


Accel::Accel(long sc, uint8_t address, float recalT)
  : scale(sc), Sens(address, recalT)
{
}