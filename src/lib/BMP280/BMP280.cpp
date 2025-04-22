#include "BMP280.h"

BMP280::BMP280(float recalT) : Baro(BMP280_ADDRESS, recalT) {}

void BMP280::begin()
{
  Wire.beginTransmission(address);
  Wire.write(BMP280_ADDRESS_COMPENSATION_L);
  Wire.endTransmission();
  Wire.requestFrom(address, uint8_t(24));

  unsigned long temp = micros();
  while (Wire.available() < 24)
  {
    if (temp + 10 < micros())
      break;
  }

  dig_T1 = Wire.read() | Wire.read() << 8; // 0x88 / 0x89
  dig_T2 = Wire.read() | Wire.read() << 8; // 0x8A / 0x8B
  dig_T3 = Wire.read() | Wire.read() << 8; // 0x8C / 0x8D
  dig_P1 = Wire.read() | Wire.read() << 8; // 0x8E / 0x8F
  dig_P2 = Wire.read() | Wire.read() << 8; // 0x90 / 0x91
  dig_P3 = Wire.read() | Wire.read() << 8; // 0x92 / 0x93
  dig_P4 = Wire.read() | Wire.read() << 8; // 0x94 / 0x95
  dig_P5 = Wire.read() | Wire.read() << 8; // 0x96 / 0x97
  dig_P6 = Wire.read() | Wire.read() << 8; // 0x98 / 0x99
  dig_P7 = Wire.read() | Wire.read() << 8; // 0x9A / 0x9B
  dig_P8 = Wire.read() | Wire.read() << 8; // 0x9C / 0x9D
  dig_P9 = Wire.read() | Wire.read() << 8; // 0x9E / 0x9F

  Wire.beginTransmission(address);
  Wire.write(BMP280_ADDERESS_CONTROL);
  Wire.write(BMP280_CONTROL);
  Wire.endTransmission();

// #if BMP280_CONFIG
  Wire.beginTransmission(address);
  Wire.write(BMP280_ADDERESS_CONFIG);
  Wire.write(BMP280_CONFIG);
  Wire.endTransmission();
// #endif // BMP280_CONFIG
}

bool BMP280::readAll()
{

  thisReadT = micros();
  Wire.beginTransmission(address);
  state = Wire.endTransmission() == 0;
  if (state)
  {
    if (getTimeLapse() > recalibrateT)
    {
      begin();
#if PRINT
      Serial.println(F("Relacibrado B"));
#endif // PRINT
    }

    Wire.beginTransmission(address);
    Wire.write(BMP280_ADDRESS_PRESS_MSB);
    Wire.endTransmission();
    Wire.requestFrom(address, uint8_t(6));

    unsigned long temp = micros();
    while (Wire.available() < 6)
    {
      if (temp + 10 < micros())
        break;
    }

    adc_p = ((uint32_t(Wire.read()) << 16) | (uint32_t(Wire.read()) << 8) | (uint32_t(Wire.read()))) >> 4; // 0xF7 / 0xF8 / 0xF9
    adc_t = ((uint32_t(Wire.read()) << 16) | (uint32_t(Wire.read()) << 8) | (uint32_t(Wire.read()))) >> 4; // 0xFA / 0xFB / 0xFC

    celcius = float(bmp280_compensate_T_int32(adc_t)) / 100.0;
    pascal = float(bmp280_compensate_P_int64(adc_p)) / 256.0;
    lastWorkT = thisReadT;
  }
  lastReadT = thisReadT;
}

BMP280_S32_t BMP280::bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
  BMP280_S32_t var1, var2, T;
  var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) * ((BMP280_S32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}

BMP280_U32_t BMP280::bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
  BMP280_S64_t var1, var2, p;
  var1 = ((BMP280_S64_t)t_fine) - 128000;
  var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
  var2 = var2 + ((var1 * (BMP280_S64_t)dig_P5) << 17);
  var2 = var2 + (((BMP280_S64_t)dig_P4) << 35);
  var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3) >> 8) + ((var1 * (BMP280_S64_t)dig_P2) << 12);
  var1 = (((((BMP280_S64_t)1) << 47) + var1)) * ((BMP280_S64_t)dig_P1) >> 33;
  if (var1 == 0)
  {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((BMP280_S64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7) << 4);
  return (BMP280_U32_t)p;
}