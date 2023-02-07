#include "Baro.h"

///Barometro
Baro::Baro(float recalT) :Sens(0x77, recalT)
{
}

void Baro::begin()
{
	Wire.beginTransmission(address);
	Wire.write(0XAA);
	Wire.endTransmission();

	Wire.requestFrom(address, (uint8_t)22);
	unsigned long temp = micros();
	while (Wire.available() < 22)
	{
		if (temp + 10 < micros())break;
	}
	ac1 = Wire.read() << 8 | Wire.read();
	ac2 = Wire.read() << 8 | Wire.read();
	ac3 = Wire.read() << 8 | Wire.read();
	ac4 = Wire.read() << 8 | Wire.read();
	ac5 = Wire.read() << 8 | Wire.read();
	ac6 = Wire.read() << 8 | Wire.read();
	b1 = Wire.read() << 8 | Wire.read();
	b2 = Wire.read() << 8 | Wire.read();
	mb = Wire.read() << 8 | Wire.read();
	mc = Wire.read() << 8 | Wire.read();
	md = Wire.read() << 8 | Wire.read();
}

bool Baro::readAll()
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
			Serial.println(F("Relacibrado B"));
#endif // PRINT
		}
		//Leitura de temperatura

		// Write 0x2E into Register 0xF4
		// This requests a temperature reading
		Wire.beginTransmission(address);
		Wire.write(0xF4);
		Wire.write(0x2E);
		Wire.endTransmission();

		// Wait at least 4.5ms
		delayMicroseconds(4550);

		// Read two bytes from registers 0xF6 and 0xF7
		Wire.beginTransmission(address);
		Wire.write(0xF6);
		Wire.endTransmission();

		Wire.requestFrom(address, (uint8_t)2);
		unsigned long temp = micros();
		while (Wire.available() < 2)
		{
			if (temp + 10 < micros())break;
		}
		ut = Wire.read() << 8 | Wire.read();

		//Leitura de pressao

		// Write 0x34+(OSS<<6) into register 0xF4
		// Request a pressure reading w/ oversampling setting
		Wire.beginTransmission(address);
		Wire.write(0xF4);
		Wire.write(0x34 + (OSS << 6));
		Wire.endTransmission();

		// Wait for conversion, delay time dependent on OSS
		delay(2 + (3 << OSS));

		// Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
		Wire.beginTransmission(address);
		Wire.write(0xF6);
		Wire.endTransmission();

		Wire.requestFrom(address, (uint8_t)3);
		temp = micros();
		while (Wire.available() < 3)
		{
			if (temp + 10 < micros())break;
		}

		up = (((unsigned long)Wire.read() << 16) | ((unsigned long)Wire.read() << 8) | (unsigned long)Wire.read()) >> (8 - OSS);

		//Garante que todas as leituras foram feitas
		Wire.beginTransmission(address);
		state = Wire.endTransmission() == 0;
		if (state)
		{

			//Calculo de temperatura
			b5 = ((((long)ut - (long)ac6)*(long)ac5) >> 15) + ((long)mc << 11) / (((((long)ut - (long)ac6)*(long)ac5) >> 15) + md);
			celcius = (float)((b5 + 8) >> 4) / 10;

			//Calculo de pressao
			b6 = b5 - 4000;
			// Calcula B3
			x1 = (b2 * (b6 * b6) >> 12) >> 11;
			x2 = (ac2 * b6) >> 11;
			x3 = x1 + x2;
			b3 = (((((long)ac1) * 4 + x3) << OSS) + 2) >> 2;

			// Calcula B4
			x1 = (ac3 * b6) >> 13;
			x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
			x3 = ((x1 + x2) + 2) >> 2;
			b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

			b7 = ((unsigned long)(up - b3) * (50000 >> OSS));
			if (b7 < 0x80000000) pascal = (b7 << 1) / b4;
			else pascal = (b7 / b4) << 1;

			x1 = (pascal >> 8) * (pascal >> 8);
			x1 = (x1 * 3038) >> 16;
			x2 = (-7357 * pascal) >> 16;
			pascal += (x1 + x2 + 3791) >> 4;

			lastWorkT = thisReadT;
		}
	}
	lastReadT = thisReadT;
	return state;
}

float Baro::getTemperature()
{
	return celcius;
}

long Baro::getPressure()
{
	return pascal;
}