#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS.h>
#include "Classes.h"

/////////////////////////////////////////////////CONFIGURATION/////////////////////////////////////////////////

#define GY80 1		//Use GY80 sensor
#define BMP085 1	//Use BMP085 sensor
#define SDCard 1	//Use SD card
#define GPSmode 1	//Use GPS
#define LoRamode 1	//Serial mode for transmission on LoRa module
#define PRINT 1		//Print or not things on Serial

#define Pbmp 1		//Print barometer informations
#define Pacel 1		//Print acelerometer informations
#define Pgiro 1		//Print gyroscope informations
#define Pmag 1		//Print magnetometer informations
#define Papg 1		//Print apogee informations

#define Tcom 1		//Print time counter
#define Lcom 1		//Print loop counter
#define MaxCond 1	//Print maximum apogee coefficient detected

//////////////////////////////////////////////////////POO//////////////////////////////////////////////////////

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
#define APGutil APGutilitario

#define SDutil SDutilitario
#define SDC SecureDigitalCard

#define GpS GlobalPSystem
#define GpSerial Serial2

#define LoRa Serial3
#define LRutil LRutilitario

#define Serial Serial

#define util utilitario


#if BMP085 || GY80
Baro bmp;
MediaMovel MM_bmp[2]{ (10),(10) };
Apogeu apg(10, 50, 50);
DuDeploy rec(11, 12, 10, 9, 5, 15);
Helpful APGutil;
#define LapsMaxT 3	//seconds
#endif // BMP085 || GY80

#if GY80
Acel acele;
MediaMovel MM_acel[3]{ (10),(10),(10) };
Giro giros(2000);
MediaMovel MM_giro[3]{ (10),(10),(10) };
Mag megnt;
MediaMovel MM_mag[3]{ (10),(10),(10) };
#endif // GY80

#if SDCard
Helpful SDutil;
SDCardHelper SDC(53, "Tupa");
#endif // SDCard

#if GPSmode
TinyGPS GpS;
unsigned short sentences, failed;
unsigned char sat;
float flat, flon;
unsigned long prec, chars;
bool newGps = false;
bool GyGPS();
#endif // GPSmode

#if LoRamode
Helpful LRutil;
#endif // LoRamode

Helpful util;

/////////////////////////////////////////////////////SETUP/////////////////////////////////////////////////////

void setup()
{
	pinMode(13, OUTPUT);
#if PRINT
	Serial.begin(250000);
#endif // Serial

#if LoRamode
	Serial3.begin(9600);
	LRutil.begin();
#endif // LoRamode

#if SDCard
	SDC.begin();
	if (SDC)
	{
		SDC.print("titulo");
		SDC.close();
	}
	SDutil.begin();
#endif // SDCard

#if GPSmode
	GpSerial.begin(9600);
#endif // GPSmode

#if BMP085 || GY80 
	Wire.begin();
	bmp.begin();
	bmp.readAll();
	bmp.readZero(100);
	apg.resetTimer();
	rec.setP2height(-100);
	rec.begin();
	APGutil.begin();
#endif // BMP085 || GY80

#if GY80
	giros.begin();
	megnt.begin();
	acele.begin();
#endif // GY80

}

/////////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////

void loop()
{
	util.counter();
#if Tcom
	if (APGutil.eachN(100))
	{
		Serial.println(APGutil.lapse(), 5);
	}
	else
	{
		APGutil.lapse();
	}
#endif // Tcom

#if Lcom
	Serial.print(util.getCount());
	Serial.print(":\t");
#endif // Lcom

#if BMP085 || GY80
	if (bmp)
	{
		MM_bmp[0].addValor(bmp.getTemperature());
		MM_bmp[1].addValor(bmp.getPressure());
	}
	apg.addAltitude(bmp.getAltitude());
#endif // BMP085 || GY80

#if GY80
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
#endif // GY80


#if MBP085 || GY80
	rec.emergency(bmp.getTimeLapse() > 1000000 * LapsMaxT);
	if (rec.getSysState(0))
	{
		apg.apgSigma();
		apg.apgAlpha();
		rec.sealApogee(apg.getApogeu(0.9f));
		rec.refresh(bmp.getAltitude());
#if MaxCond
		APGutil.comparer(apg.getSigma());
#endif // MaxCond
	}
	analogWrite(13, (int)(apg.getSigma() * 255));
#endif // MBP085 || GY80

#if GPSmode
	GyGPS();
#endif // GPSmode

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

#endif // GY80
#if BMP085 || GY80
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
#if MaxCond
	Serial.print(APGutil.getMax(), 5);
	Serial.print('\t');
#endif // MaxCond
#endif // Papg  
#endif // BMP085 || GY80

	Serial.println();
#endif // PRINT

#if SDCard
	if (!SDutil.mem)
	{
		if (SDC)
		{
#if GY80
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
#endif // GY80

			SDC.printab(APGutil.sinceBegin(), 3);
#if GY80
			for (int i = 0; i < 3; i++) SDC.printab(MM_acel[i], 3);
			for (int i = 0; i < 3; i++) SDC.printab(MM_giro[i], 1);
			for (int i = 0; i < 3; i++) SDC.printab(MM_mag[i], 1);
#endif // GY80
#if BMP085 || GY80
			for (int i = 0; i < 2; i++) SDC.printab(MM_bmp[i]);
			SDC.printab(apg.getAltutude());
#endif // BMP085 || GY80

#if GPSmode
			if (newGps)
			{
				SDC.printab(flat, 6);//Latitude
				SDC.printab(flon, 6);//Longitude
				SDC.printab(sat);//Número de satelites
				SDC.printab(prec);//Precisão
			}
			else
			{
				SDC.printab();
				SDC.printab();
				SDC.printab();
				SDC.printab();
			}
			SDC.printab(chars);//Número de caracteres recebidos
			SDC.printab(sentences);//Sentensas escritas
			SDC.printab(failed);//Número de erros
#endif // GPSmode

#if BMP085 || GY80
			if (apg.getApogeu(0.9, 0))
			{
				SDC.print("Apogeu: altitude - ");
				SDC.print(apg.getApgPt());
				SDC.print(" m, tempo - ");
				SDC.print(apg.getApgTm());
				SDC.printab(" s");
			}
			if (rec.getSysState())
			{
				if (rec.getP1S(0)) SDC.printab("Acionamento 1");
				if (rec.getP2S(0)) SDC.printab("Acionamento 2");
			}
#endif // BMP085 || GY80
			SDC.println();
			SDC.close();
		}
		else SDutil.mem = 1;
	}
	else if (SDutil.eachT(15)) if (SDC.begin()) SDutil.mem = 0;
#endif // SDCard

#if LoRamode
	LRutil.counter();
	if (LRutil.eachT(2))
	{
#if GPSmode
		LoRa.print(flat, 6);//Latitude
		LoRa.print('\t');
		LoRa.print(flon, 6);//Longitude
		LoRa.print('\t');
		LoRa.print(sat);//Número de satelites
		LoRa.print('\t');
		LoRa.print(prec);//Precisão
		LoRa.print('\t');
#endif // GPSmode
#if BMP085 || GY80
		LoRa.print(apg.getAltutude());
		LoRa.print('\t');
		LoRa.print(apg.getSigma());
		LoRa.print('\t');
#endif // BMP085 || GY80
#if !GPSmode && !(BMP085 || GY80)
		LoRa.print(LRutil.getCount());
#endif // !GPSmode && !GY80
		LoRa.println();
	}
#endif // LoRamode
}

//////////////////////////////////////////////////////GPS//////////////////////////////////////////////////////

#if GPSmode
bool GyGPS()
{ //Gps
	newGps = false;
	// Recebe as informações do GPS durante um intervalo de tempo relativamente curto e as transmite via comunicação serial (visível pelo serial monitor)
	if (GpSerial.available()) while (GpSerial.available())
	{
		char c = GpSerial.read();
		if (GpS.encode(c)) newGps = true;
	}
	if (newGps)
	{
		unsigned long age;
		GpS.f_get_position(&flat, &flon, &age);
		sat = GpS.satellites();
		prec = GpS.hdop();
	}
	GpS.stats(&chars, &sentences, &failed);
	return newGps;
}
#endif // GPSmode

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
