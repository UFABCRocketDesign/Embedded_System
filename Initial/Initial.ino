#include <Wire.h>
#include "Classes.h"

#define Pbmp 1
#define Pacel 0
#define Pgiro 0
#define Pmag 0





Baro bmp;
Giro giros(2000);
Mag megnt;
Acel acele;
MediaMovel MM_bmp[3]{ (10),(10),(10) };
MediaMovel MM_acel[3]{ (10),(10),(10) };
MediaMovel MM_giro[3]{ (10),(10),(10) };
MediaMovel MM_mag[3]{ (10),(10),(10) };
Apogeu apg(10, 100, 50);
DuDeploy rec(12, 11, 10, 9, 10, 5);

float cond = 0, maxCond = 0;
long time = 0, cont = 0, C0 = 0, T0 = 0;

void setup()
{
	pinMode(13, OUTPUT);
	Wire.begin();
	Serial.begin(250000);
	Serial3.begin(9600);
	bmp.begin();
	bmp.readAll();
	bmp.readZero(100);
	giros.begin();
	megnt.begin();
	acele.begin();
	Serial.println(bmp.getZero());
	apg.resetTimer();
	Serial.println();
}

void loop()
{
	/*
	time = T0;
	T0 = micros();
	if (cont - C0 >= 100)
	{
		C0 = cont;
		Serial.print(cont);
		Serial.print(":\t");
		Serial.println((T0 - time) / 1000000.0, 6);
	}
	cont++;//*/
	if (bmp.readAll())
	{
		MM_bmp[0].addValor(bmp.getTemperature());
		MM_bmp[1].addValor(bmp.getPressure());
		apg.addAltitude(bmp.getAltitude());
	}
	if (giros.readAll())
	{
		MM_giro[0].addValor(giros.getX());
		MM_giro[1].addValor(giros.getY());
		MM_giro[2].addValor(giros.getZ());
	}
	if (megnt.readAll())
	{
		MM_mag[0].addValor(megnt.getX());
		MM_mag[1].addValor(megnt.getY());
		MM_mag[2].addValor(megnt.getZ());
	}
	if (acele.readAll())
	{
		MM_acel[0].addValor(acele.getX());
		MM_acel[1].addValor(acele.getY());
		MM_acel[2].addValor(acele.getZ());
	}

	cond = apg.apgSigma();
	maxCond = (maxCond < cond) ? cond : maxCond;

	Serial.print("##############");
	////////////////////////////////////////////////////
	if (Pgiro)
	{
		Serial.print(MM_giro[0].getMedia(), 1);
		Serial.print('\t');
		Serial.print(MM_giro[1].getMedia(), 1);
		Serial.print('\t');
		Serial.print(MM_giro[2].getMedia(), 1);
		Serial.print("\t|");
	}
	////////////////////////////////////////////////////
	if (Pmag)
	{
		Serial.print(MM_mag[0].getMedia(), 1);
		Serial.print('\t');
		Serial.print(MM_mag[1].getMedia(), 1);
		Serial.print('\t');
		Serial.print(MM_mag[2].getMedia(), 1);
		Serial.print("\t|");

	}
	////////////////////////////////////////////////////
	if (Pacel)
	{
		Serial.print(MM_acel[0].getMedia(), 3);
		Serial.print('\t');
		Serial.print(MM_acel[1].getMedia(), 3);
		Serial.print('\t');
		Serial.print(MM_acel[2].getMedia(), 3);
		Serial.print("\t|");
	}
	/////////////////////////////////////////////////////
	if (Pbmp)
	{
		Serial.print(MM_bmp[0].getMedia());
		Serial.print('\t');
		Serial.print(MM_bmp[1].getMedia());
		Serial.print('\t');
		Serial.print(apg.getAltutude());
		Serial.print("\t|");
		Serial.print(apg.getMinH());
		Serial.print('\t');
		Serial.print(apg.getMaxH());
		Serial.print("\t|");
	}
	Serial.print(apg.apgAlpha());
	Serial.print('\t');
	Serial.print(cond, 7);
	Serial.print('\t');
	Serial.print(maxCond, 5);
	Serial.print("##############");
	Serial.println();
	/*
	analogWrite(13, (int)(cond * 255));
	long T = micros();
	if (T - time > 2000000)
	{
		time = T;
		Serial3.print(cont);
		Serial3.print('\t');
		Serial3.println(cond);
		cont++;
	}//*/
}