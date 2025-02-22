#include "HMC5883.h"

///Magnetometro
HMC5883::HMC5883(long recalT) : Magn(0x1E, recalT)
{
}

void HMC5883::begin()
{
	Wire.beginTransmission(address);
	Wire.write(0x02);                 // Seleciona o modo
	Wire.write(0x00);                 // Modo de medicao continuo
	Wire.endTransmission();
}

bool HMC5883::readAll()
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
			Serial.println(F("Relacibrado M"));
#endif // PRINT
		}
		///Faz a leitura de todos os eixos///
		Wire.beginTransmission(address);
		Wire.write(0x03);							//Xmsb register
		Wire.endTransmission();
		Wire.requestFrom(address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() << 8 | Wire.read();		//X_msb << 8 | X_lsb
			Z = Wire.read() << 8 | Wire.read();		//Z_msb << 8 | Z_lsb
			Y = Wire.read() << 8 | Wire.read();		//Y_msb << 8 | Y_lsb
		}
		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}