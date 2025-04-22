#include "Accel.h"


Accel::Accel(long sc, uint8_t address, long recalT)
  : scale(sc), Sens(address, recalT)
{
}