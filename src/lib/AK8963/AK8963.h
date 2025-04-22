#pragma once

#ifndef _AK8963_h
#define _AK8963_h

#include "../Magn/Magn.h"

#define MPU9250_ADDRESS_M 0b01101000

#define MPU9250_MAG_ADDRESS 0b00001100

#define MPU9250_ADDRESS_INT_PIN_CFG 0x37
#define MPU9250_ADDRESS_PWR_MGMT_1 0x6B

#define MPU9250_ADDRESS_MAG_XOUT_L 0x03

#define MPU9250_ADDRESS_MAG_ASAX 0x10
#define MPU9250_ADDRESS_MAG_CONFIG 0x0A

#define MPU9250_ADDRESS_MAG_CONFIG_FUSE 0x0F
#define MPU9250_ADDRESS_MAG_CONFIG_PWRDWN 0x00
#define MPU9250_ADDRESS_MAG_CONFIG_CMM1 0x02
#define MPU9250_ADDRESS_MAG_CONFIG_CMM2 0x06


class AK8963 : public Magn
{
private:
uint8_t asax_mag;
uint8_t asay_mag;
uint8_t asaz_mag;

int16_t xaux_mag;
int16_t yaux_mag;
int16_t zaux_mag;

public:
	AK8963(long recalT = 0.1);
	void begin();
	bool readAll();
};

#endif