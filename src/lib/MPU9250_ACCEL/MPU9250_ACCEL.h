#pragma once

#ifndef _MPU9250_ACCEL_h
#define _MPU9250_ACCEL_h

#include "../Accel/Accel.h"

#define MPU9250_ADDRESS_A 0b01101000

#define MPU9250_ADDRESS_ACC_CONFIG 0x1C

#define MPU9250_ADDRESS_ACCEL_XOUT_H 0x3B

class MPU9250_ACCEL : public Accel
{
private:
    int16_t xaux_acc;
    int16_t yaux_acc;
    int16_t zaux_acc;

    uint8_t accelConfigValue;
    float adcToMpS2;
    static constexpr float ACCEL_G = 9.80665f;  // constante universal

    void updateAccelConfig();

public:
    MPU9250_ACCEL(long sc, long recalT = 0.1);
    void begin();
    bool readAll();
};

#endif