#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <TinyGPS.h>
#include "Classes.h"

/////////////////////////////////////////////////CONFIGURATION/////////////////////////////////////////////////

#define GY80 1							//Use GY80 sensor
#define BMP085 GY80 || 1				//Use BMP085 sensor
#define ADXL345 GY80 || 1				//Use ADXL345 sensor
#define L3G4200D GY80 || 1				//Use L3G4200D sensor
#define HMC5883 GY80 || 1				//Use HMC5883 sensor
#define SDCard 1						//Use SD card
#define GPSmode 1						//Use GPS
#define LoRamode 1						//Serial mode for transmission on LoRa module
#define PRINT 1							//Print or not things on Serial
#define RBF 1							//Revome Before Flight
#define BuZZ 1							//Buzzer mode


#define ApoGee BMP085 && 1				//Detection of apogee


#define Pbmp PRINT && BMP085 && 1		//Print barometer informations
#define Pacel PRINT && ADXL345 && 1		//Print acelerometer informations
#define Pgiro PRINT && L3G4200D && 1	//Print gyroscope informations
#define Pmag PRINT && HMC5883 && 1		//Print magnetometer informations
#define Papg PRINT && ApoGee && 1		//Print apogee informations


#define Tcom PRINT && 1					//Print time counter
#define Lcom PRINT && 1					//Print loop counter
#define MaxCond ApoGee && 1				//Print maximum apogee coefficient detected
#define PWMapg ApoGee && 1				//Show the apogee coefficient in a LED

//////////////////////////////////////////////////////POO//////////////////////////////////////////////////////

#define baro Barometer
#define giro Gyroscope
#define magn Magnetometer
#define acel Accelerometer
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

#define rbfHelper ReBeFlight


#if BMP085
Baro baro;
MediaMovel MM_bmp[2]{ (10),(10) };
#endif // BMP085

#if ApoGee
Apogeu apg(10, 50, 50);
DuDeploy rec(11, 12, 10, 9, 5, 15);
Helpful APGutil;
#define LapsMaxT 5	//seconds  
#endif // ApoGee

#if ADXL345
Acel acel;
MediaMovel MM_acel[3]{ (10),(10),(10) };
#endif // ADXL345

#if L3G4200D
Giro giro(2000);
MediaMovel MM_giro[3]{ (10),(10),(10) };
#endif // L3G4200D

#if HMC5883
Mag magn;
MediaMovel MM_mag[3]{ (10),(10),(10) };
#endif // HMC5883

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

#if RBF
#define RBFpin 20
#endif // RBF

#if BuZZ
#define buzzPin 21
#endif // BuZZ

#if PWMapg
#define PWMout 13
#endif // PWMapg

/////////////////////////////////////////////////////SETUP/////////////////////////////////////////////////////

void setup()
{
	unsigned short sysC = 0;
#if PWMapg
	pinMode(PWMout, OUTPUT);
#endif // PWMapg

#if RBF
	pinMode(RBFpin, INPUT_PULLUP);
#endif // RBF

#if ApoGee
	rec.begin();
#endif // ApoGee

#if BuZZ
	pinMode(buzzPin, OUTPUT);
	digitalWrite(buzzPin, LOW);
#endif // BuZZ

#if PRINT
	Serial.begin(250000);
#endif // Serial

#if LoRamode
	LoRa.begin(9600);
#endif // LoRamode

#if SDCard
	SDC.begin();
	if (SDC)
	{
		sysC++;
#if LoRamode
		LoRa.println("SD start OK");
#endif // LoRamod
		SDC.print("temp\t\tacel\t\t\tgiro\t\t\tmag\t\t");
		SDC.close();
	}
#endif // SDCard

#if GPSmode
	GpSerial.begin(9600);
#endif // GPSmode

#if BMP085 
	Wire.begin();
	baro.begin();
	if (baro)
	{
		baro.readZero(100);
		sysC++;
#if LoRamode
		LoRa.println("Baro ok");
		LoRa.println(baro.getZero());
#endif // LoRamode
	}
#endif // BMP085
	
#if ApoGee
	rec.setP2height(500);
#endif // ApoGee

#if ADXL345
	acel.begin();
	if (acel)
	{
		sysC++;
#if LoRamode
		LoRa.println("Acel ok");
#endif // LoRamode
	}
#endif // ADXL345

#if L3G4200D
	giro.begin();
	if (giro)
	{
		sysC++;
#if LoRamode
		LoRa.println("Giro ok");
#endif // LoRamode
	}
#endif // L3G4200D

#if HMC5883
	magn.begin();
	if (magn)
	{
		sysC++;
#if LoRamode
		LoRa.println("Magn ok");
#endif // LoRamode
	}
#endif // HMC5883



	////////////////RBF directive////////////////

#if RBF
	bool rbf = 0;
	bool bzz = 0;
	Helpful rbfHelper;
	Helpful beeper[2*((unsigned short)(SDCard) + (unsigned short)(BMP085) + (unsigned short)(ADXL345) + (unsigned short)(L3G4200D) + (unsigned short)(HMC5883))];
	for (unsigned short i = 0; i < 10; i++) beeper[i].oneTime();
	do
	{
		rbf = digitalRead(RBFpin);
		if (rbfHelper.eachT(2))
		{
			sysC = 0;

#if SDCard
			if (SDC)
			{
				SDC.close();
				sysC++;
			}
#endif // SDCard
#if BMP085
			if (baro) sysC++;
#endif // BMP085
#if ADXL345
			if (acel) sysC++;
#endif // ADXL345
#if L3G4200D
			if (giro) sysC++;
#endif // L3G4200D
#if HMC5883
			if (magn) sysC++;
#endif // HMC5883

		}
#if LoRamode
		if (LRutil.eachT(2))
		{
			LoRa.print(sysC);
			LoRa.print(" parts working, ");
			LoRa.println("Waiting...");
		}
#endif // LoRamode

#if BuZZ
		if (beeper[0].eachT(2))
		{
			beeper[0].forT(0.1);
			beeper[0].oneTimeReset();
			digitalWrite(buzzPin, bzz = 1);
		}
		////////////////////////////////////////
		for (unsigned short i = 0; i < sysC * 2; i++)
		{
			if (!beeper[i].forT()) if (beeper[i].oneTime())
			{
				digitalWrite(buzzPin, bzz = !bzz);
				if (i + 1 == sysC) continue;
				beeper[i + 1].forT(0.1);
				beeper[i + 1].oneTimeReset();
			}
		}
		if (!beeper[sysC * 2 - 1].forT()) if (beeper[sysC * 2 - 1].oneTime()) digitalWrite(buzzPin, bzz = !bzz);
		///////////////////////////////////////
		/*
		if (!beeper[0].forT())
			if (beeper[0].oneTime())
			{
				digitalWrite(buzzPin, LOW);
				beeper[1].forT(0.1);
				beeper[1].oneTimeReset();
			}
		if (!beeper[1].forT())
			if (beeper[1].oneTime())
			{
				digitalWrite(buzzPin, HIGH);
				beeper[2].forT(0.1);
				beeper[2].oneTimeReset();
			}
		*/
		///////////////////////////////////////
#endif // BuZZ

		/*if (rbfHelper.eachT(0.5))
		{
			rbfHelper.forT(.1);
			rbfHelper.oneTimeReset();
			digitalWrite(buzzPin, HIGH);
		}
		if (!rbfHelper.forT()) if (rbfHelper.oneTime()) digitalWrite(buzzPin, LOW);
		*/
	} while (!rbf);
#if BuZZ
	digitalWrite(buzzPin, LOW);
#endif // BuZZ
#endif // RBF

	////////////////RBF directive////////////////

#if LoRamode
	LRutil.begin();
#endif // LoRamode

#if ApoGee
	apg.resetTimer();
	APGutil.begin();
	rec.begin();
#endif // ApoGee

#if SDCard
	SDutil.begin();
#endif // SDCard
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

#if BMP085
	if (baro)
	{
		MM_bmp[0].addValor(baro.getTemperature());
		MM_bmp[1].addValor(baro.getPressure());
	}
#endif // BMP085

#if ApoGee
	apg.addAltitude(baro.getAltitude());
#endif // ApoGee

#if ADXL345
	if (acel)
	{
		MM_acel[0].addValor(acel.getX());
		MM_acel[1].addValor(acel.getY());
		MM_acel[2].addValor(acel.getZ());
	}
#endif // ADXL345
#if L3G4200D
	if (giro)
	{
		MM_giro[0].addValor(giro.getX());
		MM_giro[1].addValor(giro.getY());
		MM_giro[2].addValor(giro.getZ());
	}
#endif // L3G4200D
#if HMC5883
	if (magn)
	{
		MM_mag[0].addValor(magn.getX());
		MM_mag[1].addValor(magn.getY());
		MM_mag[2].addValor(magn.getZ());
	}
#endif // HMC5883


#if ApoGee
	rec.emergency(baro.getTimeLapse() > 1000000 * LapsMaxT);
	if (rec.getSysState(0))
	{
		apg.apgSigma();
		apg.apgAlpha();
		rec.sealApogee(apg.getApogeu(0.9f));
		rec.refresh(baro.getAltitude());
#if MaxCond
		APGutil.comparer(apg.getSigma());
#endif // MaxCond
	}
#endif // ApoGee
#if PWMapg
	analogWrite(PWMout, (int)(apg.getSigma() * 255));
#endif // PWMapg

#if GPSmode
	GyGPS();
#endif // GPSmode



#if Pacel	////////////////////////////////////////////////////
	Serial.print(MM_acel[0], 3);
	Serial.print('\t');
	Serial.print(MM_acel[1], 3);
	Serial.print('\t');
	Serial.print(MM_acel[2], 3);
	Serial.print('\t');
#endif // Pacel
#if Pgiro	/////////////////////////////////////////////////////
	Serial.print('|');
	Serial.print(MM_giro[0], 1);
	Serial.print('\t');
	Serial.print(MM_giro[1], 1);
	Serial.print('\t');
	Serial.print(MM_giro[2], 1);
	Serial.print('\t');
#endif // Pgiro
#if Pmag	////////////////////////////////////////////////////
	Serial.print('|');
	Serial.print(MM_mag[0], 1);
	Serial.print('\t');
	Serial.print(MM_mag[1], 1);
	Serial.print('\t');
	Serial.print(MM_mag[2], 1);
	Serial.print('\t');
#endif // Pmag
#if Pbmp	////////////////////////////////////////////////////
	Serial.print('|');
	Serial.print(MM_bmp[0]);
	Serial.print('\t');
	Serial.print(MM_bmp[1]);
	Serial.print('\t');
#endif // Pbmp 
#if Papg	/////////////////////////////////////////////////////
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
	//Serial.print('|');
	Serial.print(apg.getAlpha());
	Serial.print('\t');
	Serial.print(apg.getSigma(), 7);
	Serial.print('\t');
#endif // Papg  
#if MaxCond	/////////////////////////////////////////////////////
	Serial.print(APGutil.getMax(), 5);
	Serial.print('\t');
#endif // MaxCond
#if PRINT	/////////////////////////////////////////////////////
	Serial.println();
#endif // PRINT

#if SDCard
	if (!SDutil.mem)
	{
		if (SDC)
		{
#if ADXL345
			if (acel)
			{
				MM_acel[0].addValor(acel.getX());
				MM_acel[1].addValor(acel.getY());
				MM_acel[2].addValor(acel.getZ());
			}
#endif // ADXL345
#if L3G4200D
			if (giro)
			{
				MM_giro[0].addValor(giro.getX());
				MM_giro[1].addValor(giro.getY());
				MM_giro[2].addValor(giro.getZ());
			}
#endif //L3G4200D
#if HMC5883
			if (magn)
			{
				MM_mag[0].addValor(magn.getX());
				MM_mag[1].addValor(magn.getY());
				MM_mag[2].addValor(magn.getZ());
			}
#endif // HMC5883

			SDC.printab(SDutil.sinceBegin(), 3);

#if ADXL345
			for (int i = 0; i < 3; i++) SDC.printab(MM_acel[i], 3);
#endif // ADXL345
#if L3G4200D
			for (int i = 0; i < 3; i++) SDC.printab(MM_giro[i], 1);
#endif // L3G4200D
#if HMC5883
			for (int i = 0; i < 3; i++) SDC.printab(MM_mag[i], 1);
#endif // HMC5883
#if BMP085
			for (int i = 0; i < 2; i++) SDC.printab(MM_bmp[i]);
			SDC.printab(apg.getAltutude());
#endif // BMP085

#if GPSmode
			if (newGps)
			{
				SDC.printab(flat, 6);//Latitude
				SDC.printab(flon, 6);//Longitude
				SDC.printab(sat);//Numero de satelites
				SDC.printab(prec);//Precisao
			}
			else
			{
				SDC.printab();
				SDC.printab();
				SDC.printab();
				SDC.printab();
			}
			SDC.printab(chars);//Numero de caracteres recebidos
			SDC.printab(sentences);//Sentensas escritas
			SDC.printab(failed);//Numero de erros
#endif // GPSmode

#if ApoGee
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
#endif // ApoGee
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
		LoRa.print(sat);//Nemero de satelites
		LoRa.print('\t');
		LoRa.print(prec);//Precisao
		LoRa.print('\t');
#endif // GPSmode
#if ApoGee
		LoRa.print(apg.getAltutude());
		LoRa.print('\t');
		LoRa.print(apg.getSigma());
		LoRa.print('\t');
#endif // ApoGee
#if !GPSmode && !ApoGee
		LoRa.print(LRutil.getCount());
#endif // !GPSmode && !ApoGee
		LoRa.println();
	}
#endif // LoRamode
}

//////////////////////////////////////////////////////GPS//////////////////////////////////////////////////////

#if GPSmode
bool GyGPS()
{ //Gps
	newGps = false;
	// Recebe as informa��es do GPS durante um intervalo de tempo relativamente curto e as transmite via comunica��o serial (vis�vel pelo serial monitor)
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
