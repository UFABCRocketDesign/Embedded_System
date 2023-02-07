#include "Giro.h"

///Giroscopio
Giro::Giro(long sc, long recalT) : scale(sc), Sens(0x69, recalT)
{
}

void Giro::begin()
{
	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x20);							// send register address ctrl_1
	Wire.write(0b00001111);						// send value to write
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x21);							// send register address ctrl_2
	Wire.write(0b00000000);						// send value to write
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x22);							// send register address ctrl_3
	Wire.write(0b00001000);						// send value to write
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x23);							// send register address ctrl_4
	switch (scale)
	{ // Configura o L3G4200 para 200, 500 ou 2000 graus/seg
	case 250: Wire.write(0b00000000); break;	// send value to write
	case 500: Wire.write(0b00010000); break;	// send value to write
	default:  Wire.write(0b00110000); break;	// send value to write
	}
	Wire.endTransmission();						// end transmission

	Wire.beginTransmission(address);			// start transmission to device
	Wire.write(0x24);							// send register address ctrl_5
	Wire.write(0b00000000);						// send value to write
	Wire.endTransmission();						// end transmission
}

bool Giro::readAll()
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
		///Faz a leitura de todos os eixos///
		Wire.beginTransmission(address);
		Wire.write(0x28 | (1 << 7));				//Xlsb register
		Wire.endTransmission();
		Wire.requestFrom(address, (uint8_t)6);
		if (6 <= Wire.available())
		{
			X = Wire.read() | (Wire.read() << 8);
			Y = Wire.read() | (Wire.read() << 8);
			Z = Wire.read() | (Wire.read() << 8);
		}
		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}