#include "ADXL345.h"

///Acelerometro
ADXL345::ADXL345(long recalT) : Accel(0x53, recalT)
{
}

void ADXL345::begin()
{
	Wire.beginTransmission(address);// enable to measute g data
	Wire.write(0x2D);
	Wire.write(8);                    //measuring enable
	Wire.endTransmission();           // stop transmitting

	Wire.beginTransmission(address);// enable to measute g data
	Wire.write(0x31);
	Wire.endTransmission();
	Wire.requestFrom(address, (uint8_t)1);
	unsigned long temp = micros();
	while (!Wire.available())
	{
		if (temp + 10 < micros())break;
	}
	char B = Wire.read();
	B &= ~0x0F;
	B |= 0b11;
	B |= 8;
	Wire.beginTransmission(address);
	Wire.write(0x31);
	Wire.write(B);                    //measuring enable
	Wire.endTransmission();           // stop transmitting
}

bool ADXL345::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(address);
	state = Wire.endTransmission() == 0;
	///Faz a leitura de todos os eixos///
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
		Wire.write(0x32);							//Xlsb register
		Wire.endTransmission();
		Wire.requestFrom(address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() | Wire.read() << 8;		//X_lsb + X_msb << 8
			Y = Wire.read() | Wire.read() << 8;		//Y_lsb + Y_msb << 8
			Z = Wire.read() | Wire.read() << 8;		//Z_lsb + Z_msb << 8
		}
		X *= 0.004 * 9.80665F;
		Y *= 0.004 * 9.80665F;
		Z *= 0.004 * 9.80665F;

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}