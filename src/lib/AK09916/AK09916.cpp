#include "AK09916.h"

AK09916::AK09916(float recalT) : Magn(ICM20948_ADDRESS_M, recalT)
{
}

void AK09916::begin()
{
  Wire.beginTransmission(0x68);
  Wire.write(REG_BANK_SEL);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(0x03);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(0x68);
  Wire.write(INT_PIN_CFG);
  Wire.write(0x02);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(MAG_CNTL3);
  Wire.write(0x01);
  Wire.endTransmission();

  delayMicroseconds(500);

  Wire.beginTransmission(address);
  Wire.write(MAG_CNTL2);
  Wire.write(0x08);
  Wire.endTransmission();
}

bool AK09916::readAll() 
{
  Wire.beginTransmission(address);
  Wire.write(MAG_HXL);
  Wire.endTransmission(false);

  Wire.requestFrom(address, uint8_t(8));

  unsigned long timeout = micros();
  while (Wire.available() < 8) {
    if (micros() - timeout > 1000) return false;
  }

  int16_t x_raw = Wire.read() | (Wire.read() << 8);
  int16_t y_raw = Wire.read() | (Wire.read() << 8);
  int16_t z_raw = Wire.read() | (Wire.read() << 8);
  Wire.flush();
  // Wire.read();

  X = float(x_raw) * MAG_SENSITIVITY;
  Y = float(y_raw) * MAG_SENSITIVITY;
  Z = float(z_raw) * MAG_SENSITIVITY;

  return true;
}