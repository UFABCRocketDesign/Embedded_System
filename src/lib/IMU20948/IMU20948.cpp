#include "IMU20948.h"

bool IMU20948::begin() {
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

