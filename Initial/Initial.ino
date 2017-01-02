#include <TinyGPS.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "Classes.h"

/////////////////////////////////////////////////CONFIGURATION/////////////////////////////////////////////////

#define GY80 1								//Use GY80 sensor
#define BMP085 GY80 || 1					//Use BMP085 sensor
#define ADXL345 GY80 || 0					//Use ADXL345 sensor
#define L3G4200D GY80 || 0					//Use L3G4200D sensor
#define HMC5883 GY80 || 0					//Use HMC5883 sensor
#define SDCard 1							//Use SD card
#define GPSmode 1							//Use GPS
#define LoRamode 1							//Serial mode for transmission on LoRa module
#define PRINT 1								//Print or not things on Serial
#define RBF 1								//Revome Before Flight
#define BuZZ 1								//Buzzer mode

#define ApoGee BMP085 & 1					//Detection of apogee

#define Pbmp PRINT & BMP085 & 0				//Print barometer informations
#define Pacel PRINT & ADXL345 & 1			//Print acelerometer informations
#define Pgiro PRINT & L3G4200D & 1			//Print gyroscope informations
#define Pmag PRINT & HMC5883 & 1			//Print magnetometer informations
#define Papg PRINT & ApoGee & 1				//Print apogee informations
#define Pgps PRINT & GPSmode & 1			//Print GPS informations
#define Psep PRINT & 0						//Print visual separator


#define Tcom PRINT & 0						//Print time counter
#define Lcom PRINT & 0						//Print loop counter
#define MaxCond ApoGee & PRINT & 0			//Print maximum apogee coefficient detected
#define PWMapg ApoGee & 1					//Show the apogee coefficient in a LED

#define LoRaDelay 4

#define WIREmode BMP085 || ADXL345 || L3G4200D || HMC5883
#define SYSTEM_n ((short)(SDCard)+(short)(BMP085)+(short)(ADXL345)+(short)(L3G4200D)+(short)(HMC5883)+(short)(GPSmode))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////objects///////////////////////////////////////////////////

#if BMP085
#define baro Barometer
#define MM_bmp M_bmp
Baro baro;									//Barometer object declaration
MediaMovel MM_bmp[2]{ (10),(10) };			//Array declaration of the moving average filter objects
#endif // BMP085

#if ApoGee
#define rec Recover
#define apg Apogee
#define APGutil APGutilitario
Apogeu apg(5, 30, 50);						//Apogee checker object declaration
DuDeploy rec(8, 10, 6, 5, 5, 15);			//Dual deployment parachute object declaration
Helpful APGutil;							//Declaration of helpful object to peak detection.
#define LapsMaxT 5							//Maximum time of delay until emergency state declaration by the delay in sensor response. (seconds)  
#define p2h 500								//Height to main parachute
#endif // ApoGee

#if ADXL345
#define acel Accelerometer
#define MM_acel M_acel
Acel acel;									//Acelerometer object declaration
MediaMovel MM_acel[3]{ (10),(10),(10) };	//Array declaration of the moving average filter objects
#endif // ADXL345

#if L3G4200D
#define giro Gyroscope
#define MM_giro M_giro
Giro giro(2000);							//Gyroscope object declaration
MediaMovel MM_giro[3]{ (10),(10),(10) };	//Array declaration of the moving average filter objects
#endif // L3G4200D

#if HMC5883
#define magn Magnetometer
#define MM_magn M_magn
Magn magn;									//Magnetometer object declaration
MediaMovel MM_magn[3]{ (10),(10),(10) };	//Array declaration of the moving average filter objects
#endif // HMC5883

#if SDCard
#define SDC SecureDigitalCard
SDCH SDC(53, "Tupa");				//Declaration of object to help SD card file management
#endif // SDCard

#if GPSmode
#define GpS GlobalPSystem
GyGPS GpS(0);
#endif // GPSmode

#if LoRamode
#define LoRa Serial3
#define LRutil LRutilitario
Helpful LRutil;								//Declaration of helpful object to telemetry system
#endif // LoRamode

#if PRINT
#define Serial Serial
#endif // PRINT

#define Gutil utilitario
Helpful Gutil;								//Declaration of helpful object to general cases

#if RBF
#if BuZZ
#define holdT .05
#endif // BuZZ

#define rbfHelper ReBeFlight
#define RBFpin 7							//Pin that the RBF system is connected
unsigned short sysC = 0;
#endif // RBF

#if BuZZ
#define buzzPin 2							//Pin that the buzzer is connected
#define buzzCmd HIGH						//Buzzer is on in high state
#endif // BuZZ

#if PWMapg
#define PWMout 13							//Pin that the LED who shows apogee state is connected
#endif // PWMapg

/////////////////////////////////////////////////////SETUP/////////////////////////////////////////////////////

void setup()
{
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
	digitalWrite(buzzPin, !buzzCmd);
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
#if RBF
		sysC++;
#endif // RBF

#if PRINT
		Serial.print("SD start OK ");
		Serial.println(SDC.getFname());
#endif // PRINT
#if LoRamode
		LoRa.print("SD start OK ");
		LoRa.println(SDC.getFname());
#endif // LoRamod
		SDC.theFile.print("temp\t");
#if ADXL345
		SDC.theFile.print("\tacel\t\t");
#endif // ADXL345
#if L3G4200D
		SDC.theFile.print("\tgiro\t\t");
#endif // L3G4200D
#if HMC5883
		SDC.theFile.print("\tmag\t\t");
#endif // HMC5883
#if BMP085
		SDC.theFile.print("\tbaro\t\t");
#endif // BMP085
#if GPSmode
		SDC.theFile.print("\t\t\tGPS\t\t\t");
#endif // GPSmode
		SDC.theFile.println();

		SDC.theFile.print("seg\t");
#if ADXL345
		SDC.theFile.print("X\tY\tZ\t");
#endif // ADXL345
#if L3G4200D
		SDC.theFile.print("X\tY\tZ\t");
#endif // L3G4200D
#if HMC5883
		SDC.theFile.print("X\tY\tZ\t");
#endif // HMC5883
#if BMP085
		SDC.theFile.print((char)0xB0);
		SDC.theFile.print("C\tPascal\tm\t");
#endif // BMP085
#if GPSmode
		SDC.theFile.print("Latitude\tLongitude\tAltutude (m)\tspeed\tSat\tPrec\t");
#endif // GPSmode
		SDC.theFile.println();
		SDC.close();
	}
	else
	{
#if PRINT
		Serial.println("SD err");
#endif // PRINT
#if LoRamode
		LoRa.println("SD err");
#endif // LoRamode
	}
#endif // SDCard

#if GPSmode
	GpS.begin();
	if (GpS)
	{
		if (GpS.util.oneTime())
		{
#if RBF
			sysC++;
#endif // RBF
#if PRINT
			Serial.print("GPS ok ");
			Serial.print(GpS.getLatitude(), 6);
			Serial.print(", ");
			Serial.print(GpS.getLongitude(), 6);
#endif // PRINT

#if LoRamode
			LoRa.print("GPS ok ");
			LoRa.print(GpS.getLatitude(), 6);
			LoRa.print(", ");
			LoRa.print(GpS.getLongitude(), 6);
#endif // LoRamode
		}
	}
	else
	{
#if PRINT
		Serial.println("GPS err, waiting signal");
#endif // PRINT
#if LoRamode
		LoRa.println("GPS err, waiting signal");
#endif // LoRamode
	}
#endif // GPSmode

#if WIREmode
	Wire.begin();
#endif // WIREmode

#if BMP085 
	baro.begin();
	if (baro)
	{
		for (int i = 0; i < 100; i++)
		{
			if (baro) apg.addZero(baro.getPressure());
		}
		//baro.readZero(100);
#if RBF
		sysC++;
#endif // RBF
#if PRINT
		Serial.print("Baro ok ");
		Serial.println(apg.getZero());
#endif // PRINT

#if LoRamode
		LoRa.print("Baro ok ");
		LoRa.println(apg.getZero());
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println("Baro err");
#endif // PRINT
#if LoRamode
		LoRa.println("Baro err");
#endif // LoRamode
	}
#endif // BMP085

#if ApoGee
	rec.setP2height(p2h);
#endif // ApoGee

#if ADXL345
	acel.begin();
	if (acel)
	{
#if RBF
		sysC++;
#endif // RBF
#if PRINT
		Serial.println("Acel ok");
#endif // PRINT

#if LoRamode
		LoRa.println("Acel ok");
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println("Acel err");
#endif // PRINT

#if LoRamode
		LoRa.println("Acel err");
#endif // LoRamode
	}
#endif // ADXL345

#if L3G4200D
	giro.begin();
	if (giro)
	{
#if RBF
		sysC++;
#endif // RBF
#if PRINT
		Serial.println("Giro ok");
#endif // PRINT
#if LoRamode
		LoRa.println("Giro ok");
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println("Giro err");
#endif // PRINT
#if LoRamode
		LoRa.println("Giro err");
#endif // LoRamode
	}
#endif // L3G4200D

#if HMC5883
	magn.begin();
	if (magn)
	{
#if RBF
		sysC++;
#endif // RBF
#if PRINT
		Serial.println("Magn ok");
#endif // PRINT
#if LoRamode
		LoRa.println("Magn ok");
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println("Magn err");
#endif // PRINT
#if LoRamode
		LoRa.println("Magn err");
#endif // LoRamode
	}
#endif // HMC5883



	////////////////RBF directive////////////////

#if RBF
	RemoveBefore();
#endif // RBF

	////////////////RBF directive////////////////

#if PRINT
	Serial.print("temp\t");
#if ADXL345
	Serial.print("\tacel\t\t");
#endif // ADXL345
#if L3G4200D
	Serial.print("\tgiro\t\t");
#endif // L3G4200D
#if HMC5883
	Serial.print("\tmag\t\t");
#endif // HMC5883
#if BMP085
	Serial.print("\tbaro\t\t");
#endif // BMP085
#if Papg
	Serial.print("\t\t\t");
#endif // Papg
#if MaxCond
	Serial.print("\t");
#endif // MaxCond
#if GPSmode
	Serial.print("\t\t\tGPS\t\t\t");
#endif // GPSmode
	Serial.println();

	Serial.print("seg\t");
#if ADXL345
	Serial.print("X\tY\tZ\t");
#endif // ADXL345
#if L3G4200D
	Serial.print("X\tY\tZ\t");
#endif // L3G4200D
#if HMC5883
	Serial.print("X\tY\tZ\t");
#endif // HMC5883
#if BMP085
	Serial.print((char)0xB0);
	Serial.print("C\tPascal\tm\t");
#endif // BMP085
#if Papg
	Serial.print("Max\tAlpha\tSigma\t");
#endif // Papg
#if MaxCond
	Serial.print("Max S\t");
#endif // MaxCond
#if GPSmode
	Serial.print("Latitude\tLongitude\tAltitude\tSpeed (m/s)\tSat\tPrec\t");
#endif // GPSmode
	Serial.println();

#endif // PRINT

	Gutil.begin();

#if LoRamode
	LRutil.begin();
#endif // LoRamode

#if ApoGee
	apg.resetTimer();
	APGutil.begin();
	rec.resetTimer();
#endif // ApoGee

#if SDCard
	SDC.util.begin();
#endif // SDCard
}

/////////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////

void loop()
{
	Gutil.counter();
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
	Serial.print(Gutil.getCount());
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
	apg.calcAlt(baro.getPressure());
	//apg.addAltitude(baro.getAltitude());
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
		MM_magn[0].addValor(magn.getX());
		MM_magn[1].addValor(magn.getY());
		MM_magn[2].addValor(magn.getZ());
	}
#endif // HMC5883


#if ApoGee
	rec.emergency(baro.getTimeLapse() > 1000000 * LapsMaxT);
	if (rec.getSysState(0))
	{
		//Serial.print("S:");
		apg.apgSigma();// Serial.print(' ');
		//Serial.print("A:");
		apg.apgAlpha();// Serial.print('\t');
		rec.sealApogee(apg.getApogeu(0.9f));
		rec.refresh(apg.getAltitude());
#if MaxCond
		APGutil.comparer(apg.getSigma());
#endif // MaxCond
	}
#if BuZZ
	else if (APGutil.oneTime()) pinMode(buzzPin, HIGH);
#endif // BuZZ
#endif // ApoGee
#if PWMapg
	analogWrite(PWMout, (int)(apg.getSigma() * 255));
#endif // PWMapg

#if GPSmode
	GpS.readAll();
#endif // GPSmode


#if PRINT
	Serial.print(Gutil.sinceBegin(), 3);
	Serial.print('\t');
#endif // PRINT

#if Pacel	////////////////////////////////////////////////////
#if Psep
	Serial.print('|');
#endif // Psep
	Serial.print(MM_acel[0], 3);
	Serial.print('\t');
	Serial.print(MM_acel[1], 3);
	Serial.print('\t');
	Serial.print(MM_acel[2], 3);
	Serial.print('\t');
#endif // Pacel
#if Pgiro	/////////////////////////////////////////////////////
#if Psep
	Serial.print('|');
#endif // Psep
	Serial.print(MM_giro[0], 1);
	Serial.print('\t');
	Serial.print(MM_giro[1], 1);
	Serial.print('\t');
	Serial.print(MM_giro[2], 1);
	Serial.print('\t');
#endif // Pgiro
#if Pmag	////////////////////////////////////////////////////
#if Psep
	Serial.print('|');
#endif // Psep
	Serial.print(MM_magn[0], 1);
	Serial.print('\t');
	Serial.print(MM_magn[1], 1);
	Serial.print('\t');
	Serial.print(MM_magn[2], 1);
	Serial.print('\t');
#endif // Pmag
#if Pbmp	////////////////////////////////////////////////////
#if Psep
	Serial.print('|');
#endif // Psep
	Serial.print(MM_bmp[0]);
	Serial.print('\t');
	Serial.print(MM_bmp[1]);
	Serial.print('\t');
#endif // Pbmp 
#if Papg	/////////////////////////////////////////////////////
	Serial.print(apg.getAltitude());
	Serial.print('\t');
#if Psep
	Serial.print('|');
#endif // Psep
	//Serial.print(apg.getMinH());
	//Serial.print('\t');
	//Serial.print(apg.getMaxH());
	//Serial.print('\t');
	Serial.print(apg.getApgPt());
	Serial.print('\t');
	//Serial.print(apg.getApgTm());
	//Serial.print('\t');
	//#if Psep
	//	Serial.print('|');
	//#endif // Psep
	Serial.print(apg.getAlpha());
	Serial.print('\t');
	Serial.print(apg.getSigma(), 7);
	Serial.print('\t');
#endif // Papg  
#if MaxCond	/////////////////////////////////////////////////////
#if Psep
	Serial.print('|');
#endif // Psep
	Serial.print(APGutil.getMax(), 5);
	Serial.print('\t');
#endif // MaxCond
#if Pgps
#if Psep
	Serial.print('|');
#endif // Psep
	Serial.print(GpS.getLatitude(), 6);
	Serial.print('\t');
	Serial.print(GpS.getLongitude(), 6);
	Serial.print('\t');
	Serial.print(GpS.getAltitude());
	Serial.print('\t');
	Serial.print(GpS.getMps(), 3);
	Serial.print('\t');
	Serial.print(GpS.getSatellites());
	Serial.print('\t');
	Serial.print(GpS.getPrecision());
	Serial.print('\t');
	Serial.print(GpS.getHour());
	Serial.print('\t');
	Serial.print(GpS.getMinute());
	Serial.print('\t');
#endif // Pgps
#if Papg
	if (apg.getApogeu(0.9, 0))
	{
#if BuZZ
		digitalWrite(buzzPin, 1);
#endif // BuZZ

		Serial.print("Apogeu: altitude - ");
		Serial.print(apg.getApgPt());
		Serial.print(" m, tempo - ");
		Serial.print(apg.getApgTm());
		Serial.print(" s\t");
	}
	if (rec.getSysState())
	{
		if (rec.getP1S(0)) Serial.print("Acionamento 1\t");
		if (rec.getP2S(0)) Serial.print("Acionamento 2\t");
	}
#endif // Papg

#if PRINT	/////////////////////////////////////////////////////
	Serial.println();
#endif // PRINT

#if SDCard
	if (!SDC.util.mem)
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
				MM_magn[0].addValor(magn.getX());
				MM_magn[1].addValor(magn.getY());
				MM_magn[2].addValor(magn.getZ());
			}
#endif // HMC5883

			SDC.theFile.print(SDC.util.sinceBegin(), 3); SDC.tab();

#if ADXL345
			for (int i = 0; i < 3; i++) { SDC.theFile.print(MM_acel[i], 3); SDC.tab(); }
#endif // ADXL345
#if L3G4200D
			for (int i = 0; i < 3; i++) { SDC.theFile.print(MM_giro[i], 1); SDC.tab(); }
#endif // L3G4200D
#if HMC5883
			for (int i = 0; i < 3; i++) { SDC.theFile.print(MM_magn[i], 1); SDC.tab(); }
#endif // HMC5883
#if BMP085
			for (int i = 0; i < 2; i++) { SDC.theFile.print(MM_bmp[i]); SDC.tab(); }
			SDC.theFile.print(apg.getAltitude()); SDC.tab();
#endif // BMP085

#if GPSmode
			if (GpS.isNew())
			{
				SDC.theFile.print(GpS.getLatitude(), 6);	SDC.tab(); //Latitude
				SDC.theFile.print(GpS.getLongitude(), 6);	SDC.tab(); //Longitude
				SDC.theFile.print(GpS.getAltitude());		SDC.tab(); //Altitude
				SDC.theFile.print(GpS.getMps());			SDC.tab(); //Velocidade
				SDC.theFile.print(GpS.getSatellites());		SDC.tab(); //Numero de satelites
				SDC.theFile.print(GpS.getPrecision());		SDC.tab(); //Precisao
			}
			else
			{
				SDC.tab();
				SDC.tab();
				SDC.tab();
				SDC.tab();
				SDC.tab();
				SDC.tab();
			}
#endif // GPSmode

#if ApoGee
			if (apg.getApogeu(0.9, 0))
			{
				SDC.theFile.print("Apogeu: altitude - ");
				SDC.theFile.print(apg.getApgPt());
				SDC.theFile.print(" m, tempo - ");
				SDC.theFile.print(apg.getApgTm());
				SDC.theFile.print(" s");
				SDC.tab();
			}
			if (rec.getSysState())
			{
				if (rec.getP1S(0)) SDC.theFile.print("Acionamento 1");
				if (rec.getP2S(0)) SDC.theFile.print("Acionamento 2");
			}
#endif // ApoGee
			SDC.theFile.println();
			SDC.close();
		}
		else SDC.util.mem = 1;
	}
	else if (SDC.util.eachT(15)) if (SDC.begin()) SDC.util.mem = 0;
#endif // SDCard

#if LoRamode
	LRutil.counter();
	if (LRutil.eachT(LoRaDelay))
	{
#if GPSmode
		LoRa.print(GpS.getLatitude(), 6);//Latitude
		LoRa.print('\t');
		LoRa.print(GpS.getLongitude(), 6);//Longitude
		LoRa.print('\t');
		LoRa.print(GpS.getHour());//Hora
		LoRa.print(':');
		LoRa.print(GpS.getMinute());//Minuto
		LoRa.print('\t');
		LoRa.print(GpS.getPrecision());//Precisao
		LoRa.print('\t');
#endif // GPSmode
#if ApoGee
		LoRa.print(apg.getAltitude());
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

//////////////////////////////////////////////////////RBF//////////////////////////////////////////////////////

#if RBF
void RemoveBefore()
{
	bool rbf = 0;
	bool bzz = 0;
	Helpful rbfHelper;
	Helpful beeper[SYSTEM_n * 2 + 1];
	for (short i = 0; i < SYSTEM_n * 2 + 1; i++) beeper[i].oneTime();
	do
	{
		rbf = digitalRead(RBFpin);
#if GPSmode
		if (GpS) if (GpS.util.oneTime()) sysC++;
#endif // GPSmode

		if (rbfHelper.eachT(2))
		{
			sysC = 0;
#if GPSmode
			GpS.util.oneTimeReset();
#endif // GPSmode

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
#if PRINT
			Serial.print(sysC);
			Serial.print(" parts of ");
			Serial.print(SYSTEM_n);
			Serial.print(" working, waiting...\t");
			Serial.print(baro.getTemperature());
			Serial.print(" ");
			Serial.write(0xB0);
			Serial.print("C\tLat: ");
			Serial.print(GpS.getLatitude(), 6);
			Serial.print("\tLon: ");
			Serial.print(GpS.getLongitude(), 6);
			Serial.print("\t");
			Serial.print(GpS.getDay());
			Serial.print('/');
			Serial.print(GpS.getMonth());
			Serial.print('\t');
			Serial.print(GpS.getHour());
			Serial.print(':');
			Serial.print(GpS.getMinute());
			Serial.print(':');
			Serial.print(GpS.getSecond());
			Serial.println();

#endif // PRINT
		}
#if LoRamode
		if (LRutil.eachT(LoRaDelay))
		{
			LoRa.print(sysC);
			LoRa.print(" parts of ");
			LoRa.print(SYSTEM_n);
			LoRa.print(" working, waiting...\t");
			LoRa.print(baro.getTemperature());
			LoRa.print(" ");
			LoRa.write(0xB0);
			LoRa.print("C\tLat: ");
			LoRa.print(GpS.getLatitude(), 6);
			LoRa.print("\tLon: ");
			LoRa.print(GpS.getLongitude(), 6);
			LoRa.print("\t");
			LoRa.print(GpS.getDay());
			LoRa.print('/');
			LoRa.print(GpS.getMonth());
			LoRa.print('\t');
			LoRa.print(GpS.getHour());
			LoRa.print(':');
			LoRa.print(GpS.getMinute());
			LoRa.print(':');
			LoRa.print(GpS.getSecond());
			LoRa.println();
		}
#endif // LoRamode

#if BuZZ
		///////////////////////////////////////
		if (beeper[0].eachT(holdT*SYSTEM_n * 2 + 1))
		{
			beeper[0].oneTimeReset();
			beeper[0].forT(holdT);
			digitalWrite(buzzPin, bzz = buzzCmd);
		}
		for (short i = 0; i < sysC * 2; i++) if (!beeper[i].forT()) if (beeper[i].oneTime())
		{
			digitalWrite(buzzPin, bzz = !bzz);
			beeper[i + 1].oneTimeReset();
			beeper[i + 1].forT(holdT);
		}
		if (!beeper[sysC * 2].forT()) if (beeper[sysC * 2].oneTime())digitalWrite(buzzPin, bzz = !bzz);
		///////////////////////////////////////
#endif // BuZZ

	} while (!rbf);
#if BuZZ
	digitalWrite(buzzPin, LOW);
#endif // BuZZ
}
#endif // RBF

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
