#pragma once

#ifndef _BMP280_H
#define _BMP280_H

#include "../Baro/Baro.h"
#include <Wire.h>

#define BMP280_ADDRESS 0b01110110

#define BMP280_ADDRESS_COMPENSATION_L 0x88
#define BMP280_ADDRESS_PRESS_MSB 0xF7

#define BMP280_ADDERESS_CONTROL 0xF4
#define BMP280_ADDERESS_CONFIG 0xF5

#define BMP280_CONTROL_OSRS_T 0x1
#define BMP280_CONTROL_OSRS_P 0x2
#define BMP280_CONTROL_MODE 0x3

#define BMP280_CONTROL (((BMP280_CONTROL_OSRS_T) << 5) | ((BMP280_CONTROL_OSRS_P) << 2) | (BMP280_CONTROL_MODE))

#define BMP280_CONFIG_T_SB 0x0
#define BMP280_CONFIG_FILTER 0x0

#define BMP280_CONFIG (((BMP280_CONFIG_T_SB) << 5) | ((BMP280_CONFIG_FILTER) << 2))

/* Datasheet */
#define BMP280_S32_t int32_t
#define BMP280_U32_t uint32_t
#define BMP280_S64_t int64_t


class BMP280 : public Baro
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	
	uint32_t adc_p;
	uint32_t adc_t;

	// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
	// t_fine carries fine temperature as global value
	BMP280_S32_t t_fine;

public:
    BMP280(float recalT = 0.1);
    void begin();
    bool readAll();

private:
	BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
    BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);
};

#endif
