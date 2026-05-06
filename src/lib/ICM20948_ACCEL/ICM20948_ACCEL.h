#pragma once

#ifndef _ICM20948_ACCEL_h
#define _ICM20948_ACCEL_h

// #if defined(ARDUINO) && ARDUINO >= 100
// #include "Arduino.h"
// #else
// #include "WProgram.h"
// #endif
#include <Wire.h>
#include "../Accel/Accel.h"

#define ICM20948_ADDRESS_A 0x68
#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define LP_CONFIG 0x05
// #define GYRO_CONFIG_1 0x01
// #define GYRO_SMPLRT_DIV 0x00
#define ACCEL_CONFIG 0x14
#define ACCEL_SMPLRT_DIV_1 0x10
#define ACCEL_SMPLRT_DIV_2 0x11
#define ACCEL_XOUT_H 0x2D
#define INT_PIN_CFG 0x0F
#define REG_BANK_SEL 0x7F

class ICM20948_ACCEL : public Accel
{
private:
  int16_t xaux_acc;
  int16_t yaux_acc;
  int16_t zaux_acc;

  static constexpr float ACCEL_G = 9.80665f;
  static constexpr float ACCEL_SENSITIVITY_16G = 2048.0f;

public:
  ICM20948_ACCEL(float sc, float recalT = 0.1);
  void begin();
  bool readAll();
};
#endif