#pragma once

#ifndef _IMU20948_h
#define _IMU20948_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define LP_CONFIG 0x05
#define GYRO_CONFIG_1 0x01
#define GYRO_SMPLRT_DIV 0x00
#define ACCEL_CONFIG 0x14
#define ACCEL_SMPLRT_DIV_1 0x10
#define ACCEL_SMPLRT_DIV_2 0x11
#define INT_PIN_CFG 0x0F
#define REG_BANK_SEL 0x7F

class IMU20948
{
private:
  uint8_t address = 0x69;
public:
  bool begin();
  float readAccel();
  float readGyro();
  float readMagn();
}
#endif