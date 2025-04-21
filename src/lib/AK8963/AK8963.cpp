#include "AK8963.h"

AK8963::AK8963(long recalT) : Magn(MPU9250_MAG_ADDRESS, recalT)
{
}

void AK8963::begin()
{
	uint8_t byte;
  
	Wire.beginTransmission(MPU9250_ADDRESS);
	Wire.write(MPU9250_ADDRESS_INT_PIN_CFG);
	Wire.endTransmission();
	Wire.requestFrom(MPU9250_ADDRESS, 1);
	byte = Wire.read();
	byte |= 0b00000010;
  
	Wire.beginTransmission(MPU9250_ADDRESS);
	Wire.write(MPU9250_ADDRESS_INT_PIN_CFG);
	Wire.write(byte);
	Wire.endTransmission();
  
	Wire.beginTransmission(MPU9250_MAG_ADDRESS);
	Wire.write(MPU9250_ADDRESS_MAG_CONFIG);      // Control 1
	Wire.write(MPU9250_ADDRESS_MAG_CONFIG_FUSE); // Fuse ROM
	Wire.endTransmission();
  
	Wire.beginTransmission(MPU9250_MAG_ADDRESS);
	Wire.write(MPU9250_ADDRESS_MAG_ASAX);
	Wire.endTransmission();
	Wire.requestFrom(MPU9250_MAG_ADDRESS, 3);
	asax_mag = Wire.read();
	asay_mag = Wire.read();
	asaz_mag = Wire.read();
  
	Wire.beginTransmission(MPU9250_MAG_ADDRESS);
	Wire.write(MPU9250_ADDRESS_MAG_CONFIG);      // Control 1
	Wire.write(MPU9250_ADDRESS_MAG_CONFIG_CMM2); // Continuous measurement mode 1
	Wire.endTransmission();
}

bool AK8963::readAll()
{
	thisReadT = micros();
	Wire.beginTransmission(MPU9250_MAG_ADDRESS);
	state = Wire.endTransmission() == 0;
	Wire.write(MPU9250_ADDRESS_MAG_XOUT_L);
	Wire.endTransmission();

	
  
	Wire.requestFrom(MPU9250_MAG_ADDRESS, 7);
	unsigned long temp = micros();
	while (Wire.available() < 7)
	{
	  if (temp + 10 < micros())
		break;
	}
  
	xaux_mag = Wire.read() | Wire.read() << 8; // Valor do ADC de 16 bits
	yaux_mag = Wire.read() | Wire.read() << 8; // Valor do ADC de 16 bits
	zaux_mag = Wire.read() | Wire.read() << 8; // Valor do ADC de 16 bits
	Wire.flush();                              // O datasheet indica que é necessário ler este byte para atualizar
  
	xout_mag = float(xaux_mag) * ((((float(asax_mag) - 128.0) * 0.5) / 128.0) + 1);
	yout_mag = float(yaux_mag) * ((((float(asay_mag) - 128.0) * 0.5) / 128.0) + 1);
	zout_mag = float(zaux_mag) * ((((float(asaz_mag) - 128.0) * 0.5) / 128.0) + 1);  
}