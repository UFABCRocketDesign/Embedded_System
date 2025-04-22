#include "MPU9250_ACCEL.h"

MPU9250_ACCEL::MPU9250_ACCEL(long sc, long recalT) : Accel(sc, MPU9250_ADDRESS_A, recalT)
{
}

void MPU9250_ACCEL::begin()
{
    updateAccelConfig();
    Wire.beginTransmission(address);
    Wire.write(MPU9250_ADDRESS_ACC_CONFIG);
    Wire.write(accelConfigValue);
    Wire.endTransmission();
}

bool MPU9250_ACCEL::readAll()
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
            Serial.println(F("Relacibrado A"));
#endif // PRINT
        }
        Wire.beginTransmission(address);
        Wire.write(MPU9250_ADDRESS_ACCEL_XOUT_H);
        Wire.endTransmission();

        Wire.requestFrom(address, uint8_t(6));
        unsigned long temp = micros();
        while (Wire.available() < 6)
        {
            if (temp + 10 < micros())
                break;
        }

        xaux_acc = Wire.read() << 8 | Wire.read(); // Valor do ADC de 16 bits
        yaux_acc = Wire.read() << 8 | Wire.read(); // Valor do ADC de 16 bits
        zaux_acc = Wire.read() << 8 | Wire.read(); // Valor do ADC de 16 bits

        X = float(xaux_acc) * adcToMpS2;
        Y = float(yaux_acc) * adcToMpS2;
        Z = float(zaux_acc) * adcToMpS2;

        lastWorkT = thisReadT;
    }
    lastReadT = thisReadT;
    return state;
}

void MPU9250_ACCEL::updateAccelConfig() {
    switch (scale) {
        case 2:
            accelConfigValue = 0b00000000;
            break;
        case 4:
            accelConfigValue = 0b00001000;
            break;
        case 8:
            accelConfigValue = 0b00010000;
            break;
        case 16:
        default:
            accelConfigValue = 0b00011000;
            break;
    }

    adcToMpS2 = ACCEL_G * float(scale) / float(0x1u << 15);
}