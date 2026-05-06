#include "ICM20948_GYRO.h"

ICM20948_GYRO::ICM20948_GYRO(float sc, float recalT) : Gyro(sc, ICM20948_ADDRESS_G, recalT)
{
}

void ICM20948_GYRO::begin()
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

  Wire.beginTransmission(address);
  Wire.write(GYRO_CONFIG_1);
  Wire.write(0x1F);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(GYRO_SMPLRT_DIV);
  Wire.write(0x00);
  Wire.endTransmission();

  // Wire.beginTransmission(address);
  // Wire.write(ACCEL_CONFIG);
  // Wire.write(0x1F);
  // Wire.endTransmission();

  // Wire.beginTransmission(address);
  // Wire.write(ACCEL_SMPLRT_DIV_1);
  // Wire.write(0x00);
  // Wire.endTransmission();

  // Wire.beginTransmission(address);
  // Wire.write(ACCEL_SMPLRT_DIV_2);
  // Wire.write(0x00);
  // Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(REG_BANK_SEL);
  Wire.write((0x00 & 0x03) << 4);
  Wire.endTransmission();

  Wire.beginTransmission(address);
  Wire.write(INT_PIN_CFG);
  Wire.write(0x30);
  Wire.endTransmission();
}

bool ICM20948_GYRO::readAll()
{
  Wire.beginTransmission(address);
  Wire.endTransmission();
  Wire.beginTransmission(address);
  Wire.write(GYRO_XOUT_H);
  Wire.endTransmission();

  Wire.requestFrom(address, uint8_t(6));
  unsigned long temp = micros();
  while (Wire.available() < 6) {
    if (temp + 10 < micros())
      break;
  }

  xaux_gyro = Wire.read() << 8 | Wire.read();
  yaux_gyro = Wire.read() << 8 | Wire.read();
  zaux_gyro = Wire.read() << 8 | Wire.read();

  X = float(xaux_gyro) / GYRO_SENSITIVITY_2000DPS;
  Y = float(yaux_gyro) / GYRO_SENSITIVITY_2000DPS;
  Z = float(zaux_gyro) / GYRO_SENSITIVITY_2000DPS;
  
  return true;
}