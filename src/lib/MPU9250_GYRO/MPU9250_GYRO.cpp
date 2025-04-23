#include "MPU9250_GYRO.h"

MPU9250_GYRO::MPU9250_GYRO(long sc, long recalT) : Gyro(sc, MPU9250_ADDRESS_G, recalT)
{
}

void MPU9250_GYRO::begin()
{
    updateGyroConfig();

    Wire.beginTransmission(address);
    Wire.write(MPU9250_ADDRESS_GYRO_CONFIG);
    Wire.write(gyroConfigValue);
    Wire.endTransmission();
}

bool MPU9250_GYRO::readAll()
{
    thisReadT = micros();
    Wire.beginTransmission(address);
    state = Wire.endTransmission() == 0;
    if (state)
    {
        if (getTimeLapse() > recalibrateT)
        {
            begin();
#if PRINT
            Serial.println(F("Relacibrado G"));
#endif // PRINT
        }
        Wire.beginTransmission(address);
        Wire.write(MPU9250_ADDRESS_GYRO_XOUT_H);
        Wire.endTransmission();

        Wire.requestFrom(address, uint8_t(6));
        unsigned long temp = micros();
        while (Wire.available() < 6)
        {
            if (temp + 10 < micros())
                break;
        }

        xaux_gyro = Wire.read() << 8 | Wire.read(); // Valor do ADC de 16 bits
        yaux_gyro = Wire.read() << 8 | Wire.read(); // Valor do ADC de 16 bits
        zaux_gyro = Wire.read() << 8 | Wire.read(); // Valor do ADC de 16 bits

        X = float(xaux_gyro) * adcToDps;
        Y = float(yaux_gyro) * adcToDps;
        Z = float(zaux_gyro) * adcToDps;
        lastWorkT = thisReadT;
    }
    lastReadT = thisReadT;
    return state;
}

void MPU9250_GYRO::updateGyroConfig() {
    switch (scale) {
        case 250:
            gyroConfigValue = 0b00000000;
            break;
        case 500:
            gyroConfigValue = 0b00001000;
            break;
        case 1000:
            gyroConfigValue = 0b00010000;
            break;
        case 2000:
        default:
            gyroConfigValue = 0b00011000;
            break;
    }

    adcToDps = float(scale) / float(0x1u << 15); 
}
