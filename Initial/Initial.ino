#include <Wire.h>
#include "Classes.h"


#define PRINT 1
#define Pbmp PRINT && 1
#define Pacel PRINT && 1
#define Pgiro PRINT && 1
#define Pmag PRINT && 1
#define Papg PRINT && 1
#define Tcom PRINT && 0
#define Lora 0

Baro bmp;
Giro giros(2000);
Mag megnt;
Acel acele;
MediaMovel MM_bmp[2]{ (10),(10) };
MediaMovel MM_acel[3]{ (10),(10),(10) };
MediaMovel MM_giro[3]{ (10),(10),(10) };
MediaMovel MM_mag[3]{ (10),(10),(10) };
Apogeu apg(10, 50, 50);
DuDeploy rec(12, 11, 10, 9, 5, 15);

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
	Serial.println(bmp.getZero());

	giros.begin();
	megnt.begin();
	acele.begin();

	apg.resetTimer();
	rec.setP2height(-100);
	rec.setP1height(-50);
	rec.begin();
	Serial.println();
}

void loop()
{
	//delay(250);
#if Tcom
	time = T0;
	T0 = micros();
	if (cont - C0 >= 100)
	{
		C0 = cont;
		Serial.print(cont);
		Serial.print(":\t");
		Serial.println((T0 - time) / 1000000.0, 6);
	}
#endif // Tcom

	cont++;
	Serial.print(cont);
	Serial.print('\t');

	if (bmp.readAll())
	{
		MM_bmp[0].addValor(bmp.getTemperature());
		MM_bmp[1].addValor(bmp.getPressure());
	}
	if (acele.readAll())
	{
		MM_acel[0].addValor(acele.getX());
		MM_acel[1].addValor(acele.getY());
		MM_acel[2].addValor(acele.getZ());
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

	apg.addAltitude(bmp.getAltitude());

	rec.emergency(bmp.getTimeLapse() > 5000000);

	if (rec.getSysState(0))
	{
		cond = apg.apgSigma();
		rec.sealApogee(cond > 0.9);
		rec.refresh(bmp.getAltitude());
		maxCond = (maxCond < cond) ? cond : maxCond;
	}

	analogWrite(13, (int)(cond * 255));



	//Serial.println(bmp.getTimeLapse()/1000000.0,5);
#if Pgiro || Pmag || Pacel || Pbmp  || Papg
	//Serial.print("####\t");
	//Serial.print(cont);
	//Serial.print(":\t");
#endif // Pgiro || Pmag || Pacel || Pbmp  || Papg
	////////////////////////////////////////////////////
#if Pacel
	Serial.print(MM_acel[0].getMedia(), 3);
	Serial.print('\t');
	Serial.print(MM_acel[1].getMedia(), 3);
	Serial.print('\t');
	Serial.print(MM_acel[2].getMedia(), 3);
	Serial.print("\t|");
#endif // Pacel
	/////////////////////////////////////////////////////
#if Pgiro
	Serial.print(MM_giro[0].getMedia(), 1);
	Serial.print('\t');
	Serial.print(MM_giro[1].getMedia(), 1);
	Serial.print('\t');
	Serial.print(MM_giro[2].getMedia(), 1);
	Serial.print("\t|");
#endif // Pgiro
	////////////////////////////////////////////////////
#if Pmag
	Serial.print(MM_mag[0].getMedia(), 1);
	Serial.print('\t');
	Serial.print(MM_mag[1].getMedia(), 1);
	Serial.print('\t');
	Serial.print(MM_mag[2].getMedia(), 1);
	Serial.print("\t|");
#endif // Pmag
	////////////////////////////////////////////////////
#if Pbmp
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
#endif // Pbmp
	/////////////////////////////////////////////////////
#if Papg
	Serial.print(apg.apgAlpha()*100);
	Serial.print('\t');
	Serial.print(cond, 7);
	Serial.print('\t');
	Serial.print(maxCond, 5);
#endif // Papg

#if Pgiro || Pmag || Pacel || Pbmp  || Papg
	//Serial.print("\t####");
	Serial.println();
#endif // Pgiro || Pmag || Pacel || Pbmp  || Papg
	
#if Lora
	long T = micros();
	if (T - time > 2000000)
	{
		time = T;
		Serial3.print(cont);
		Serial3.print('\t');
		Serial3.println(cond);
		cont++;
	}
#endif // Lora
}


/*
                                                                                                                      .
                                                                                                                       :::::...
                                                                                                                       888888Z~::.   .
                                                                                                                       .8888O$888D?:,.
                                                                                                                        .88Z:.Z8:.888~~
                                                                                                                          I8O:  .?8~.I8+: ..
                                                                                                                          ..88~:  .:88:88~:, .
                                                                                                                             ,88~:  .D888888O~:~..
                                                                                                                             ...88+:. .$888D.O888+::..
                                                                                                                                ..D8O::. .D888DOI8888=,.
                                                                                                                                  ..8888=::.8888Z+.  .
                                                                                                                                    .O8..?88.+=:..
                                                                                                                                      .88..  .?????=..
                                                                                                                                       .~88O...,??????.
                                                                                                                                          +7..8 .??????:
                                                                                                                                            .Z8$ ,???????
                                                                                                                                          .  . .. .=?????+.
                           .                                    .                                      .                  .       .   . .          .,?????+.
   8888888.         8888888~      88888888888888888888.          788888888             .8888888888888888..               ..788888888888: .           :?????:
   8888888.         8888888~      88888888888888888888          .888888888+.           .8888888888888888888Z..       . .8888888888888888888           ??????.
   8888888.         8888888~      88888888888888888888          88888888888.           .888888888888888888888         888888888888888888888           ,?????.
   8888888.         8888888~      88888888888888888888         .888888888888.          .8888888~~~~7888888888.       88888888888....8888888           .??????
   8888888.         8888888~      8888888                      8888888888888.          .8888888     . 8888888.    ..888888888.         8888            +?????..
   8888888.         8888888~      8888888                     :888888.8888888          .8888888       8888888..    .88888888.            =8            ,?????,
   8888888.         8888888~      8888888                    .8888888.8888888.         .8888888      .888888.      88888888                           ..?????~
   8888888.         8888888~      8888888888888888888        8888888...8888888         .8888888888888888888.       8888888~                            .?????+
   8888888.         8888888~      8888888888888888888       .8888888.  I888888? .      .8888888888888888888.       8888888.                           . ??????
   8888888.         8888888~      8888888888888888888      .8888888     8888888.       .888888888888888888888=.    8888888.                             ??????
   8888888.         8888888~      8888888............      .8888888 .   .8888888.      .8888888     ..888888888    8888888.                            .??????
   8888888.         8888888~      8888888                 .888888888888888888888.      .8888888        .8888888.   88888888.                           .??????.
   8888888.   .     8888888.      8888888                 I8888888888888888888888      .8888888         8888888.   .8888888O             .8            ,?????+
  .88888888.       :8888888.      8888888               ..88888888888888888888888,     .8888888        Z8888888.   .888888888.         .888            +?????:
   .88888888..   .88888888$       8888888               .8888888:..........8888888.    .8888888.  ...$888888888     .8888888888~... .888888            ??????.
   ..888888888888888888888        8888888               .8888888.         .8888888O    .8888888888888888888888.       888888888888888888888           ,??????
      888888888888888888. .       8888888               8888888~.          .8888888.   .88888888888888888888           .8888888888888888888           ??????:
        .8888888888888.          .8888888              .8888888             88888888   .88888888888888888..              .:888888888888$.          ...??????.
            .   .               .                          . .                                  .                    ..          .                  .??????+
                                .888888Z                    888               .88.      .$888888.                    .88.                           =??????.
                                .88  .888   .+8$..   ..$8:..888. ...   .$8:  ..88...     $88...888.   .$8~. ....88~.  ...  .$8....  ....8+.        ~??????:
                                .88  .888  888$888..:888888 888 888. .88~+88..88888..    $88  . 888 .88=:88$ 8887788  88..88888888 .8888888 .     .??????+
       .,. .  .                 .888888: ..88 . 888 88..  . 888888.  88888888 .88 .      $88    888 88888888 8888$.   88..88.  888 .88  .88.  .  .???????.
          .,, .                 .88..888  .88.  888.88.   . 888:88.. 88...... .88  .     $88  ..88O 88......  .88888. 88. 88.. 888 .88  .88.    ,???????.
             . ,~. .            .88  .888  888$888..~888888.888 .88. .888$888..8888.     $8888888.  .888Z888 88O?888  88. O8888888..88  .88.. .=???????.
            . .   .~=..         ...  ..... ..?8$..    .$8~..... .....  .?8$.   .,?~.     ........     .?8$... .$8?.   ... .... 88? ...  .... .????????.
                      .~+,.                                                                                               .888888.         .=????????.
                      . . .~?~..                                                                                         . ..  .         ..????????=.
                              ..+?+. .     .                                                                                          ..,?????????..
                                  ..:??+,. .                                                                                         .~?????????:..
                                     ...,+??+:.                                                                                  . .??????????=..
                                       .. ...:+???+,..                                                                          .+??????????~.
                                                ...~?????+:..                                                              .,+???????????+.
                                                      ..,+??????+:,..                                                 ..:??????????????,...
                                                             .:+?????????+:,...   .  .         .               ..,~+???????????????+, .
                                                               .. ...~????????????????++=~::,,,,,,,:::=++???????????????????????,...
                                                                        .  ..,~+?????????????????????????????????????????+:....
                                                                            .     . ....,~=+????????????????????+~,...  ..
                                                                                         .  ..   . .... .. ...  .
                                                                                                     .      . .
*/
