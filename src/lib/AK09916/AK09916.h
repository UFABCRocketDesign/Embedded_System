#pragma once

#ifndef _AK09916_h
#define _AK09916_h

#include "../Magn/Magn.h"

#define ICM20948_ADDRESS_M 0x0C
#define MAG_WIA2 0x01
#define MAG_ST1 0x10
#define MAG_HXL 0x11
#define MAG_CNTL2 0x31
#define MAG_CNTL3 0x32

#define PWR_MGMT_1 0x06
#define PWR_MGMT_2 0x07
#define LP_CONFIG 0x05
// #define GYRO_CONFIG_1 0x01
// #define GYRO_SMPLRT_DIV 0x00
// #define ACCEL_CONFIG 0x14
// #define ACCEL_SMPLRT_DIV_1 0x10
// #define ACCEL_SMPLRT_DIV_2 0x11
#define INT_PIN_CFG 0x0F
#define REG_BANK_SEL 0x7F

class AK09916 : public Magn
{
private:
  static constexpr float MAG_SENSITIVITY = 0.15f;
public:
  AK09916(float recalT = 0.1);
  void begin();
  bool readAll();
};

#endif