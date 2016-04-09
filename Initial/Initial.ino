//#include <Wire.h>
#include "Classes.h"

Baro bmp(0x77);
void setup()
{
	Serial.begin(250000);
	bmp.begin();
  /* add setup code here */

}

void loop()
{
	bmp.readAll();
	Serial.print(bmp.getTemperature());
	Serial.print('\t');
	Serial.print(bmp.getPressure());
	Serial.print('\t');
	Serial.print(bmp.getAltitude());
	Serial.print('\n');
  /* add main program code here */

}
