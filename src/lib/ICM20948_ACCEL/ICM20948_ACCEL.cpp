#include "ICM20948_ACCEL.h"

ICM20948_ACCEL::ICM20948_ACCEL(float sc, float recalT) : Accel(sc, ICM20948_ADDRESS_A, recalT)
{
}

void ICM20948_ACCEL::begin()
{
	Wire.beginTransmission(address);
	Wire.write(REG_BANK_SEL);
	Wire.write((0x00 & 0x03) << 4);
	Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(PWR_MGMT_1);
  Wire.write(0x01);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(PWR_MGMT_2);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(LP_CONFIG);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(REG_BANK_SEL);
  Wire.write((0x02 & 0x03) << 4);
  Wire.endTransmission();

  // Wire.beginTransmission(address);
  // Wire.write(GYRO_CONFIG_1);
  // Wire.write(0x1F);
  // Wire.endTransmission();

  // Wire.beginTransmission(address);
  // Wire.write(GYRO_SMPLRT_DIV);
  // Wire.write(0x00);
  // Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(ACCEL_CONFIG);
  Wire.write(0x1F);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(ACCEL_SMPLRT_DIV_1);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(ACCEL_SMPLRT_DIV_2);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(REG_BANK_SEL);
  Wire.write((0x00 & 0x03) << 4);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(INT_PIN_CFG);
  Wire.write(0x30);
  Wire.endTransmission();
}

bool ICM20948_ACCEL::readAll()
{
  Wire.beginTransmission(address);
  Wire.endTransmission();
  Wire.beginTransmission(address);
  Wire.write(ACCEL_XOUT_H);
  Wire.endTransmission();

  Wire.requestFrom(address, uint8_t(6));
  unsigned long temp = micros();
  while (Wire.available() < 6) {
    if (temp + 1000 < micros())
      return false;
  }

  xaux_acc = Wire.read() << 8 | Wire.read();
  yaux_acc = Wire.read() << 8 | Wire.read();
  zaux_acc = Wire.read() << 8 | Wire.read();

  X = float(xaux_acc / ACCEL_SENSITIVITY_16G) * ACCEL_G;
  Y = float(yaux_acc / ACCEL_SENSITIVITY_16G) * ACCEL_G;
  Z = float(zaux_acc / ACCEL_SENSITIVITY_16G) * ACCEL_G;

  return true;
}
