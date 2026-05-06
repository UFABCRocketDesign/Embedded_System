#pragma once

#ifndef _ICM20948_GYRO_h
#define _ICM20948_GYRO_h

#include <Wire.h>
#include "../Gyro/Gyro.h"

#define ICM20948_ADDRESS_G 0x68
#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define LP_CONFIG 0x05
#define GYRO_CONFIG_1 0x01
#define GYRO_SMPLRT_DIV 0x00
#define GYRO_XOUT_H 0x33
// #define ACCEL_CONFIG 0x14
// #define ACCEL_SMPLRT_DIV_1 0x10
// #define ACCEL_SMPLRT_DIV_2 0x11
// #define ACCEL_XOUT_H 0x2D
#define INT_PIN_CFG 0x0F
#define REG_BANK_SEL 0x7F

class ICM20948_GYRO : public Gyro
{
private:
  int16_t xaux_gyro;
  int16_t yaux_gyro;
  int16_t zaux_gyro;

  static constexpr float GYRO_SENSITIVITY_2000DPS = 16.4f;

public:
  ICM20948_GYRO(float sc, float recalT = 0.1);
  void begin();
  bool readAll();
};

#endif