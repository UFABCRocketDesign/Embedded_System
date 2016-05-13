#include <Wire.h>
#include "Classes.h"

Baro bmp(0x77);
MediaMovel MM_bmp[2]{ (50),(50) };
Apogeu apg(10, 20, 50);
void setup()
{
	Wire.begin();
	Serial.begin(250000);
	bmp.begin();
	bmp.readAll();
	Serial.print(bmp.getAltitude());
}

void loop()
{
	bmp.readAll();
	Serial.print(bmp.getTemperature());
	Serial.print('\t');
	Serial.print(bmp.getPressure());
	Serial.print('\t');
	Serial.print(bmp.getAltitude());
	Serial.print('\t');
	Serial.print(MM_bmp[0].addValor(bmp.getTemperature()));
	Serial.print('\t');
	Serial.print(MM_bmp[1].addValor(bmp.getPressure()));
	Serial.print('\t');
	Serial.print(apg.addAltitude(bmp.getAltitude()));
	Serial.print('\t');
	Serial.println(apg.serial());
	//delay(100);
  /* add main program code here */

}
