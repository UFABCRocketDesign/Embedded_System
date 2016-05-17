#include <Wire.h>
#include "Classes.h"

#define strg 255

Baro bmp;
Giro giros(2000);
Mag megnt;
Acel acele;
MediaMovel MM_bmp[2]{ (50),(50) };
MediaMovel MM_acel[3]{ (10),(10),(10) };
MediaMovel MM_giro[3]{ (10),(10),(10) };
MediaMovel MM_mag[3]{ (10),(10),(10) };
Apogeu apg(15, strg, 50);
DuDeploy rec(12, 11, 10, 9, 10, 5);
void setup()
{
	pinMode(13, OUTPUT);
	Wire.begin();
	Serial.begin(250000);
	bmp.begin();
	bmp.readAll();
	bmp.readZero(100);
	giros.begin();
	megnt.begin();
	acele.begin();
	Serial.println(bmp.getZero());
}

void loop()
{
	bmp.readAll();
	giros.readAll();
	megnt.readAll();
	acele.readAll();
	//////////////////////////////////////////////
	Serial.print(bmp.getTemperature(),1);
	Serial.print('\t');
	Serial.print(bmp.getPressure());
	Serial.print('\t');
	Serial.print(bmp.getAltitude());
	Serial.print("\t|");
	////////////////////////////////////////////////
	Serial.print(MM_giro[0].addValor(giros.getX()),1);
	Serial.print('\t');
	Serial.print(MM_giro[1].addValor(giros.getY()),1);
	Serial.print('\t');
	Serial.print(MM_giro[2].addValor(giros.getZ()),1);
	Serial.print("\t|");
	////////////////////////////////////////////////
	Serial.print(MM_mag[0].addValor(megnt.getX()),1);
	Serial.print('\t');
	Serial.print(MM_mag[1].addValor(megnt.getY()),1);
	Serial.print('\t');
	Serial.print(MM_mag[2].addValor(megnt.getZ()),1);
	Serial.print("\t|");
	////////////////////////////////////////////////
	Serial.print(MM_acel[0].addValor(acele.getX()),3);
	Serial.print('\t');
	Serial.print(MM_acel[1].addValor(acele.getY()),3);
	Serial.print('\t');
	Serial.print(MM_acel[2].addValor(acele.getZ()),3);
	Serial.print("\t|");
	////////////////////////////////////////////////
	Serial.print(MM_bmp[0].addValor(bmp.getTemperature()));
	Serial.print('\t');
	Serial.print(MM_bmp[1].addValor(bmp.getPressure()));
	Serial.print('\t');
	Serial.print(apg.addAltitude(bmp.getAltitude()));
	Serial.print('\t');
	int serie = apg.serial();
	float cond = (float)serie / (float)strg;
	Serial.print(cond);
	Serial.println();
	analogWrite(13, (int)((float)serie / (float)strg * 255));

	//delay(100);
  /* add main program code here */

}