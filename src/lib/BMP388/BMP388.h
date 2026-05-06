#pragma once

#ifndef _BMP388_H
#define _BMP388_H

#include "../Baro/Baro.h"
#include <Wire.h>

#define BMP388_ADDRESS_DEFAULT 0x76
#define BMP3_REG_DATA 0x04
#define BMP3_REG_PWR_CTRL 0x1B
#define BMP3_REG_OSR 0x1C
#define BMP3_REG_CALIB_DATA 0x31
#define BMP3_REG_CMD 0x7E

class BMP388 : public Baro 
{
  double t1, t2, t3;
  double p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11;
  double t_lin;

public:
  BMP388(float recalT = 0.1, uint8_t addr = BMP388_ADDRESS_DEFAULT);
  void begin();
  bool readAll();

private:
  double compensate_T(uint32_t adc_T);
  double compensate_P(uint32_t adc_P);
};
#endif