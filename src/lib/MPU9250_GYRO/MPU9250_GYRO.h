#pragma once

#ifndef _MPU9250_GYRO_h
#define _MPU9250_GYRO_h

#include "../Gyro/Gyro.h"

#define MPU9250_ADDRESS_G 0b01101000

#define MPU9250_ADDRESS_GYRO_CONFIG 0x1B

#define MPU9250_ADDRESS_GYRO_XOUT_H 0x43

class MPU9250_GYRO : public Gyro
{
private:
    int16_t xaux_gyro;
    int16_t yaux_gyro;
    int16_t zaux_gyro;

    uint8_t gyroConfigValue;
    float adcToDps;

    void updateGyroConfig();

public:
    MPU9250_GYRO(long sc, long recalT = 0.1);
    void begin();
    bool readAll();
};

#endif