#include "BMP388.h"

BMP388::BMP388(float recalT) : Baro(BMP388_ADDRESS_DEFAULT, recalT) {}

void BMP388::begin() {
  Wire.beginTransmission(address);
  Wire.write(BMP3_REG_CALIB_DATA);
  Wire.endTransmission();
  Wire.requestFrom(address, uint8_t(21));

  unsigned long temp = micros();
  while (Wire.available() < 21)
  {
    if (temp + 100 < micros())
      break;
  }

  uint8_t reg[21];
  for (int i = 0; i < 21; i++) reg[i] = Wire.read();

  t1 = (double)((uint16_t)reg[0] | (uint16_t)reg[1] << 8) / 0.00390625;  // Escala 2^-8
  t2 = (double)((uint16_t)reg[2] | (uint16_t)reg[3] << 8) / 1073741824.0;
  t3 = (double)((int8_t)reg[4]) / 281474976710656.0;
  p1 = ((double)((int16_t)(reg[5] | reg[6] << 8)) - 16384.0) / 1048576.0;
  p2 = ((double)((int16_t)(reg[7] | reg[8] << 8)) - 16384.0) / 536870912.0;
  p3 = (double)((int8_t)reg[9]) / 4294967296.0;
  p4 = (double)((int8_t)reg[10]) / 137438953472.0;
  p5 = (double)((uint16_t)reg[11] | (uint16_t)reg[12] << 8) / 0.125;
  p6 = (double)((uint16_t)reg[13] | (uint16_t)reg[14] << 8) / 64.0;
  p7 = (double)((int8_t)reg[15]) / 256.0;
  p8 = (double)((int8_t)reg[16]) / 32768.0;
  p9 = (double)((int16_t)(reg[17] | reg[18] << 8)) / 281474976710656.0;
  p10 = (double)((int8_t)reg[19]) / 281474976710656.0;
  p11 = (double)((int8_t)reg[20]) / 36893488147419103232.0;

  // Config: OSR x1 (0x00), PWR: Normal Mode + P & T Enabled (0x33)
  Wire.beginTransmission(address);
  Wire.write(BMP3_REG_OSR);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(BMP3_REG_PWR_CTRL);
  Wire.write(0x33);
  Wire.endTransmission();
}

bool BMP388::readAll() {
  unsigned long now = micros();
  Wire.beginTransmission(address);
  state = (Wire.endTransmission() == 0);

  if (state) {
    if (now - lastWorkT > recalibrateT) begin();

    Wire.beginTransmission(address);
    Wire.write(BMP3_REG_DATA);
    Wire.endTransmission();
    Wire.requestFrom(address, uint8_t(6));

    unsigned long temp = micros();
    while (Wire.available() < 6)
    {
      if (temp + 100 < micros())
        break;
    }

    uint32_t p_raw = (uint32_t)Wire.read() | (uint32_t)Wire.read() << 8 | (uint32_t)Wire.read() << 16;
    uint32_t t_raw = (uint32_t)Wire.read() | (uint32_t)Wire.read() << 8 | (uint32_t)Wire.read() << 16;

    celcius = (float)compensate_T(t_raw);
    pascal = (float)compensate_P(p_raw);
    lastWorkT = now;
  }
  return state;
}

double BMP388::compensate_T(uint32_t adc_T) {
  double partial1 = (double)adc_T - t1;
  t_lin = partial1 * t2 + (partial1 * partial1) * t3;
  return t_lin;
}

double BMP388::compensate_P(uint32_t adc_P) {
  double partial_out1 = p5 + p6 * t_lin + p7 * (t_lin * t_lin) + p8 * (t_lin * t_lin * t_lin);
  double partial_out2 = (double)adc_P * (p1 + p2 * t_lin + p3 * (t_lin * t_lin) + p4 * (t_lin * t_lin * t_lin));
  double partial_out3 = (double)adc_P * (double)adc_P * (p9 + p10 * t_lin) + (double)adc_P * (double)adc_P * (double)adc_P * p11;
  return partial_out1 + partial_out2 + partial_out3;
}