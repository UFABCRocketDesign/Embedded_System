#include <Wire.h>
#include "Classes.h"

#define GY80 1




#define PRINT 1

#define Pbmp 1
#define Pacel 1
#define Pgiro 1
#define Pmag 1
#define Papg 1

#define Tcom 1
#define Lcom 1

/*
#if SDCard	//Variáveis e declarações SD
#define CS 53   //Pino utilizado como CS para cartão SD
#define zer0 6
#define InF Informations

File InF;

char Dname[] = { "INF000.txt" };
#endif // SDcard
*/
//POO

#define bmp Barometer
#define giros Gyroscope
#define megnt Magnetometer
#define acele Accelerometer
#define rec Recover
#define apg Apogee
#define MM_bmp M_bmp
#define MM_acel M_acel
#define MM_giro M_giro
#define MM_mag M_mag
#define GYutil GYutilitario



#define Serial Serial

#define util utilitario


#if GY80
Baro bmp;
Acel acele;
Giro giros(2000);
Mag megnt;
MediaMovel MM_bmp[2]{ (10),(10) };
MediaMovel MM_acel[3]{ (10),(10),(10) };
MediaMovel MM_giro[3]{ (10),(10),(10) };
MediaMovel MM_mag[3]{ (10),(10),(10) };
Apogeu apg(10, 50, 50);
DuDeploy rec(11, 12, 10, 9, 5, 15);
Helpful GYutil;
#endif // GY80


Helpful util;

float maxCond = 0;

void setup()
{
	pinMode(13, OUTPUT);
#if Serial
		Serial.begin(250000);
#endif // Serial

#if GY80
	Wire.begin();
	bmp.begin();
	bmp.readAll();
	bmp.readZero(100);
	//Serial.println(bmp.getZero());

	giros.begin();
	megnt.begin();
	acele.begin();

	apg.resetTimer();
	rec.setP2height(-100);
	rec.begin();
	GYutil.begin();
#endif // GY80

}

void loop()
{
	util.counter();
#if Tcom
	if (GYutil.eachN(100))
	{
		Serial.println(GYutil.lapse(),5);
	}
	else
	{
		GYutil.lapse();
	}
#endif // Tcom

#if Lcom
	Serial.print(util.getCount());
	Serial.print(":\t");
#endif // Lcom


#if GY80
	if (bmp)
	{
		MM_bmp[0].addValor(bmp.getTemperature());
		MM_bmp[1].addValor(bmp.getPressure());
	}

	apg.addAltitude(bmp.getAltitude());

	if (acele)
	{
		MM_acel[0].addValor(acele.getX());
		MM_acel[1].addValor(acele.getY());
		MM_acel[2].addValor(acele.getZ());
	}
	if (giros)
	{
		MM_giro[0].addValor(giros.getX());
		MM_giro[1].addValor(giros.getY());
		MM_giro[2].addValor(giros.getZ());
	}
	if (megnt)
	{
		MM_mag[0].addValor(megnt.getX());
		MM_mag[1].addValor(megnt.getY());
		MM_mag[2].addValor(megnt.getZ());
	}


	rec.emergency(bmp.getTimeLapse() > 3000000);
	if (rec.getSysState(0))
	{
		apg.apgSigma();
		apg.apgAlpha();
		rec.sealApogee(apg.getApogeu(0.9f));
		rec.refresh(bmp.getAltitude());
		maxCond = (maxCond < apg.getSigma()) ? apg.getSigma() : maxCond;
	}

	analogWrite(13, (int)(apg.getSigma() * 255));
#endif // GY80


#if PRINT

#if GY80
	////////////////////////////////////////////////////
#if Pacel
	Serial.print(MM_acel[0], 3);
	Serial.print('\t');
	Serial.print(MM_acel[1], 3);
	Serial.print('\t');
	Serial.print(MM_acel[2], 3);
	Serial.print('\t');
#endif // Pacel
	/////////////////////////////////////////////////////
#if Pgiro
	Serial.print('|');
	Serial.print(MM_giro[0], 1);
	Serial.print('\t');
	Serial.print(MM_giro[1], 1);
	Serial.print('\t');
	Serial.print(MM_giro[2], 1);
	Serial.print('\t');
#endif // Pgiro
	////////////////////////////////////////////////////
#if Pmag
	Serial.print('|');
	Serial.print(MM_mag[0], 1);
	Serial.print('\t');
	Serial.print(MM_mag[1], 1);
	Serial.print('\t');
	Serial.print(MM_mag[2], 1);
	Serial.print('\t');
#endif // Pmag
	////////////////////////////////////////////////////
#if Pbmp
	Serial.print('|');
	Serial.print(MM_bmp[0]);
	Serial.print('\t');
	Serial.print(MM_bmp[1]);
	Serial.print('\t');
	Serial.print(apg.getAltutude());
	Serial.print("\t|");
	Serial.print(apg.getMinH());
	Serial.print('\t');
	Serial.print(apg.getMaxH());
	Serial.print('\t');
	Serial.print(apg.getApgPt());
	Serial.print('\t');
	Serial.print(apg.getApgTm());
	Serial.print('\t');
#endif // Pbmp
	/////////////////////////////////////////////////////
#if Papg
	//Serial.print('|');
	Serial.print(apg.getAlpha());
	Serial.print('\t');
	Serial.print(apg.getSigma(), 7);
	Serial.print('\t');
	Serial.print(maxCond, 5);
	Serial.print('\t');
#endif // Papg  

#endif // GY80

	Serial.println();
#endif // PRINT


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
