#include <TinyGPS.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "Classes.h"

/////////////////////////////////////////////////CONFIGURATION/////////////////////////////////////////////////

#define GY80 0								//Use GY80 sensor
#define BMP085 (GY80) || 1					//Use BMP085 sensor
#define ADXL345 (GY80) || 0					//Use ADXL345 sensor
#define L3G4200D (GY80) || 0				//Use L3G4200D sensor
#define HMC5883 (GY80) || 0					//Use HMC5883 sensor
#define SDCard 1							//Use SD card
#define GPSmode 0							//Use GPS
#define LoRamode 0							//Serial mode for transmission on LoRa module
#define PRINT 0								//Print or not things on Serial
#define RBF 0								//Revome Before Flight
#define BuZZ 1								//Buzzer mode

#define ApoGee (BMP085) && 1				//Detection of apogee

#define PbarT (PRINT) && (BMP085) && 0		//Print barometer temperature data
#define PbarP (PRINT) && (BMP085) && 0		//Print barometer pressure data

#define PaclX (PRINT) && (ADXL345) && 1		//Print accelerometer X axis data
#define PaclY (PRINT) && (ADXL345) && 1		//Print accelerometer Y axis data
#define PaclZ (PRINT) && (ADXL345) && 1		//Print accelerometer Z axis data

#define PgirX (PRINT) && (L3G4200D) && 1	//Print gyroscope x axis data
#define PgirY (PRINT) && (L3G4200D) && 1	//Print gyroscope Y axis data
#define PgirZ (PRINT) && (L3G4200D) && 1	//Print gyroscope Z axis data

#define PmagX (PRINT) && (HMC5883) && 1		//Print magnetometer x axis data
#define PmagY (PRINT) && (HMC5883) && 1		//Print magnetometer Y axis data
#define PmagZ (PRINT) && (HMC5883) && 1		//Print magnetometer Z axis data

#define PapgW (PRINT) && (ApoGee) && 0		//Print apogee information when detected
#define PapgH (PRINT) && (ApoGee) && 1		//Print altimeter data
#define PapgB (PRINT) && (ApoGee) && 0		//Print altimeter base
#define PapgP (PRINT) && (ApoGee) && 0		//Print apogee information
#define PapgA (PRINT) && (ApoGee) && 0		//Print apogee alpha
#define PapgS (PRINT) && (ApoGee) && 0		//Print apogee sigma
#define PapgM (PRINT) && (ApoGee) && 0		//Print apogee sigma max

#define Pgps (PRINT) && (GPSmode) && 1		//Print GPS informations
#define Psep (PRINT) && 0					//Print visual separator

#define Tcom (PRINT) && 0					//Print time counter
#define Lcom (PRINT) && 0					//Print loop counter
#define Ncom (PRINT) && 0					//Print eachN counter

#define PWMapg (ApoGee) && 1				//Show the apogee coefficient in a LED

#define WIREmode BMP085 || ADXL345 || L3G4200D || HMC5883
#define SYSTEM_n ((short)(SDCard)+(short)(BMP085)+(short)(ADXL345)+(short)(L3G4200D)+(short)(HMC5883)+(short)(GPSmode))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////objects///////////////////////////////////////////////////

#if BMP085
#define baro Barometer
#define MM_baro M_baro
Baro baro;									//Barometer object declaration
MediaMovel MM_baro[2]{ (2),(2) };			//Array declaration of the moving average filter objects
#endif // BMP085

#if ApoGee
#define rec Recover
#define apg Apogee
Apogeu apg(10, 15, 50);						//Apogee checker object declaration
DuDeploy rec(2, 4, 3, 7, 10, 15);			//Dual deployment parachute object declaration
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
SDCH SDC(10, "Boi");				//Declaration of object to help SD card file management
#endif // SDCard

#if GPSmode
#define GpS GlobalPSystem
GyGPS GpS(Serial2, 0);
#endif // GPSmode

#if LoRamode
#define LoRa Serial3
#define LoRaDelay 4
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
#define buzzPin 5							//Pin that the buzzer is connected
#define buzzCmd HIGH						//Buzzer is on in high state
#endif // BuZZ

#if PWMapg
#define PWMout 6							//Pin that the LED who shows apogee state is connected
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
#if RBF
	digitalWrite(buzzPin, !buzzCmd);
#endif // RBF
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
#elif BuZZ
		digitalWrite(buzzPin, buzzCmd);
#endif // RBF, BuZZ

#if PRINT
		Serial.print(F("SD start OK "));
		Serial.println(SDC.getFname());
#endif // PRINT
#if LoRamode
		LoRa.print(F("SD start OK "));
		LoRa.println(SDC.getFname());
#endif // LoRamod
		SDC.theFile.print(F("temp\t"));
#if ADXL345
		SDC.theFile.print(F("\tacel\t\t"));
#endif // ADXL345
#if L3G4200D
		SDC.theFile.print(F("\tgiro\t\t"));
#endif // L3G4200D
#if HMC5883
		SDC.theFile.print(F("\tmag\t\t"));
#endif // HMC5883
#if BMP085
		SDC.theFile.print(F("\tbaro\t\t"));
#endif // BMP085
#if ApoGee
		SDC.theFile.print('\t');
#endif // ApoGee
#if GPSmode
		SDC.theFile.print(F("\t\t\tGPS\t\t\t"));
#endif // GPSmode
		SDC.theFile.println();

		SDC.theFile.print(F("seg\t"));
#if ADXL345
		SDC.theFile.print(F("X\tY\tZ\t"));
#endif // ADXL345
#if L3G4200D
		SDC.theFile.print(F("X\tY\tZ\t"));
#endif // L3G4200D
#if HMC5883
		SDC.theFile.print(F("X\tY\tZ\t"));
#endif // HMC5883
#if BMP085
		SDC.theFile.print((char)0xB0);
		SDC.theFile.print(F("C\tPascal\t"));
#endif // BMP085
#if ApoGee
		SDC.theFile.print(F("m\t"));
#endif // ApoGee

#if GPSmode
		SDC.theFile.print(F("Latitude\tLongitude\tAltutude (m)\tspeed\tSat\tPrec\t"));
#endif // GPSmode
		SDC.theFile.println();
		SDC.close();
	}
	else
	{
#if PRINT
		Serial.println(F("SD err"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("SD err"));
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
			Serial.print(F("GPS ok "));
			Serial.print(GpS.getLatitude(), 6);
			Serial.print(F(", "));
			Serial.print(GpS.getLongitude(), 6);
#endif // PRINT

#if LoRamode
			LoRa.print(F("GPS ok "));
			LoRa.print(GpS.getLatitude(), 6);
			LoRa.print(F(", "));
			LoRa.print(GpS.getLongitude(), 6);
#endif // LoRamode
		}
	}
	else
	{
#if PRINT
		Serial.println(F("GPS err, waiting signal"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("GPS err, waiting signal"));
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
#if ApoGee
		for (int i = 0; i < 100; i++)
		{
			if (baro) apg.addZero(baro.getPressure());
		}
		//baro.readZero(100);
#endif // ApoGee
#if RBF
		sysC++;
#else
//#if BuZZ
//		digitalWrite(buzzPin, !buzzCmd);
//#endif // BuZZ

#endif // RBF
#if PRINT
		Serial.print(F("Baro ok "));
#if PapgB
		Serial.println(apg.getZero());
#endif // PaphB

#endif // PRINT

#if LoRamode
		LoRa.print(F("Baro ok "));
#if ApoGee
		LoRa.println(apg.getZero());
#endif // ApoGee

#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println(F("Baro err"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("Baro err"));
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
		Serial.println(F("Acel ok"));
#endif // PRINT

#if LoRamode
		LoRa.println(F("Acel ok"));
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println(F("Acel err"));
#endif // PRINT

#if LoRamode
		LoRa.println(F("Acel err"));
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
		Serial.println(F("Giro ok"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("Giro ok"));
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println(F("Giro err"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("Giro err"));
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
		Serial.println(F("Magn ok"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("Magn ok"));
#endif // LoRamode
	}
	else
	{
#if PRINT
		Serial.println(F("Magn err"));
#endif // PRINT
#if LoRamode
		LoRa.println(F("Magn err"));
#endif // LoRamode
	}
#endif // HMC5883

	////////////////RBF directive////////////////

#if RBF
	RemoveBefore();
#endif // RBF

#if (ApoGee) && !(RBF) && (BuZZ)
	digitalWrite(buzzPin, !buzzCmd);
#endif // (ApoGee) && !(RBF) && (BuZZ)

	////////////////RBF directive////////////////

#if Lcom
	Serial.print(F("loop\t"));
#endif // Lcom

#if Tcom
	Serial.print(F("temp\t"));
#endif // Tcom


#if (PaclX) || (PaclY) || (PaclZ)
	Serial.print(F("acel"));
#endif // (PaclX) || (PaclY) || (PaclZ)
#if PaclX
	Serial.print('\t');
#endif // PaclX
#if PaclY
	Serial.print('\t');
#endif // PaclY
#if PaclZ
	Serial.print('\t');
#endif // PaclZ


#if (PgirX) || (PgirX) || (PgirX)
	Serial.print(F("giro"));
#endif // (PgirX) || (PgirX) || (PgirX)
#if PgirX
	Serial.print('\t');
#endif // PgirX
#if PgirY
	Serial.print('\t');
#endif // PgirY
#if PgirZ
	Serial.print('\t');
#endif // PgirZ


#if (PmagX) || (PmagY) || (PmagZ)
	Serial.print(F("mag"));
#endif // (PmagX) || (PmagY) || (PmagZ)
#if PmagX
	Serial.print('\t');
#endif // PmagX
#if PmagY
	Serial.print('\t');
#endif // PmagY
#if PmagZ
	Serial.print('\t');
#endif // PmagZ


#if (PbarT) || (PbarP)
	Serial.print(F("baro"));
#endif // (PbarT) || (PbarP)
#if PbarT
	Serial.print('\t');
#endif // PbarT
#if PbarP
	Serial.print('\t');
#endif // PbarP

#if PapgH
	Serial.print('\t');
#endif // PapgH
#if PapgP
	Serial.print(F("\t\t"));
#endif // PapgP
#if PapgA
	Serial.print('\t');
#endif // PapgA
#if PapgS
	Serial.print('\t');
#endif // PapgS
#if PapgM
	Serial.print('\t');
#endif // PapgM

#if GPSmode
	Serial.print(F("\t\t\tGPS\t\t\t"));
#endif // GPSmode

#if (PbarT) || (PbarP) || (PaclX) || (PaclY) || (PaclZ) || (PgirX) || (PgirY) || (PgirZ) || (PmagX) || (PmagY) || (PmagZ) || (PapgW) || (PapgH) || (PapgP) || (PapgA) || (PapgS) || (PapgM) || (Pgps) || (Psep) || (Tcom) || (Lcom)
	Serial.println();
#endif // (PbarT) || (PbarP) || (PaclX) || (PaclY) || (PaclZ) || (PgirX) || (PgirY) || (PgirZ) || (PmagX) || (PmagY) || (PmagZ) || (PapgW) || (PapgH) || (PapgP) || (PapgA) || (PapgS) || (PapgM) || (Pgps) || (Psep) || (Tcom) || (Lcom)

#if Lcom
	Serial.print('\t');
#endif // Lcom

#if Tcom
	Serial.print(F("s\t"));
#endif // Tcom


#if PaclX
	Serial.print(F("X\t"));
#endif // PaclX
#if PaclY
	Serial.print(F("Y\t"));
#endif // PaclY
#if PaclZ
	Serial.print(F("Z\t"));
#endif // PaclZ


#if PgirX
	Serial.print(F("X\t"));
#endif // PgirX
#if PgirY
	Serial.print(F("Y\t"));
#endif // PgirY
#if PgirZ
	Serial.print(F("Z\t"));
#endif // PgirZ


#if PmagX
	Serial.print(F("X\t"));
#endif // PmagX
#if PmagY
	Serial.print(F("Y\t"));
#endif // PmagY
#if PmagZ
	Serial.print(F("Z\t"));
#endif // PmagZ


#if PbarT
	Serial.print((char)0xB0);
	Serial.print(F("C\t"));
#endif // PbarT
#if PbarP
	Serial.print(F("Pascal\t"));
#endif // PbarP

#if PapgH
	Serial.print(F("m\t"));
#endif // PapgH
#if PapgP
	Serial.print("Max h\tMax t\t");
#endif // PapgP
#if PapgA
	Serial.print(F("Alpha\t"));
#endif // PapgA
#if PapgS
	Serial.print(F("Sigma\t"));
#endif // PapgS
#if PapgM
	Serial.print(F("Max S\t"));
#endif // PapgM

#if GPSmode
	Serial.print(F("Latitude\tLongitude\tAltitude\tSpeed (m/s)\tSat\tPrec\t"));
#endif // GPSmode

#if (PbarT) || (PbarP) || (PaclX) || (PaclY) || (PaclZ) || (PgirX) || (PgirY) || (PgirZ) || (PmagX) || (PmagY) || (PmagZ) || (PapgW) || (PapgH) || (PapgP) || (PapgA) || (PapgS) || (PapgM) || (Pgps) || (Psep) || (Tcom) || (Lcom)
	Serial.println();
#endif // (PbarT) || (PbarP) || (PaclX) || (PaclY) || (PaclZ) || (PgirX) || (PgirY) || (PgirZ) || (PmagX) || (PmagY) || (PmagZ) || (PapgW) || (PapgH) || (PapgP) || (PapgA) || (PapgS) || (PapgM) || (Pgps) || (Psep) || (Tcom) || (Lcom)

	Gutil.begin();

#if LoRamode
	LRutil.begin();
#endif // LoRamode

#if ApoGee
	apg.resetTimer();
	rec.resetTimer();
//#if !(RBF) && (BuZZ)
//	if (!rec.info1()) { digitalWrite(buzzPin, !buzzCmd); Serial.println(F("IGN")); }
//#endif // !(RBF) && (BuZZ)

#endif // ApoGee

#if SDCard
	SDC.util.begin();
#endif // SDCard
}

/////////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////

void loop()
{
	Gutil.counter();
#if Ncom
	if (Gutil.eachN(100))
	{
		Serial.print(Gutil.getCount());
		Serial.print('\t');
		Serial.println(Gutil.lapse(), 5);
	}
	else
	{
		Gutil.lapse();
	}
#endif // Tcom

#if BMP085
	if (baro)
	{
		MM_baro[0].addValor(baro.getTemperature());
		MM_baro[1].addValor(baro.getPressure());
	}
#endif // BMP085

#if ApoGee
	apg.calcAlt(baro.getPressure());
	//apg.addAltitude(baro.getAltitude());
#endif // ApoGee

#if !SDCard
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
#endif // SDCard

#if ApoGee
	rec.emergency(baro.getTimeLapse() > 1000000 * LapsMaxT);
	if (rec.getSysState(0))
	{
		//Serial.print(F("S:"));
		apg.apgSigma();// Serial.print(' ');
		//Serial.print(F("A:"));
		apg.apgAlpha();// Serial.print('\t');
		rec.sealApogee(apg.getApogeu(0.9f));
		rec.refresh(apg.getAltitude());
#if PapgM
		Gutil.comparer(apg.getSigma());
#endif // PapgM
#if SDCard
		if (rec.getApogee()) Gutil.mem = 1;
#endif // SDCard
	}
#if BuZZ
	else if (Gutil.oneTime()) digitalWrite(buzzPin, buzzCmd);
#endif // BuZZ
#endif // ApoGee
#if PWMapg
	analogWrite(PWMout, (int)(apg.getSigma() * 255));
#endif // PWMapg

#if GPSmode
	GpS.readAll();
#endif // GPSmode

#if Lcom
	Serial.print(Gutil.getCount());
	Serial.print(F(":\t"));
#endif // Lcom

#if Tcom
	Serial.print(Gutil.sinceBegin(), 3);
	Serial.print('\t');
#endif // Tcom

	////////////////////////////////////////////////////
#if (Psep) && ((PaclX) || (PaclY) || (PaclZ))
	Serial.print('|');
#endif // (Psep) && ((PaclX) || (PaclY) || (PaclZ))
#if PaclX
	Serial.print(MM_acel[0], 3);
	Serial.print('\t');
#endif // PaclX

#if PaclY
	Serial.print(MM_acel[1], 3);
	Serial.print('\t');
#endif // PaclY

#if PaclZ
	Serial.print(MM_acel[2], 3);
	Serial.print('\t');
#endif // PaclZ

	/////////////////////////////////////////////////////
#if (Psep) && ((PgirX) || (PgirY) || (PgirZ))
	Serial.print('|');
#endif // (Psep) && ((PgirX) || (PgirY) || (PgirZ))
#if PgirX
	Serial.print(MM_giro[0], 1);
	Serial.print('\t');
#endif // PgirX

#if PgirY
	Serial.print(MM_giro[1], 1);
	Serial.print('\t');
#endif // PgirY

#if PgirZ
	Serial.print(MM_giro[2], 1);
	Serial.print('\t');
#endif // PgirZ

	////////////////////////////////////////////////////
#if (Psep) && ((PmagX) || (PmagY) || (PmagZ))
	Serial.print('|');
#endif // (Psep) && ((PmagX) || (PmagY) || (PmagZ))
#if PmagX
	Serial.print(MM_magn[0], 1);
	Serial.print('\t');
#endif // PmagX
#if PmagY
	Serial.print(MM_magn[1], 1);
	Serial.print('\t');
#endif // PmagY
#if PmagZ
	Serial.print(MM_magn[2], 1);
	Serial.print('\t');
#endif // PmagZ

	////////////////////////////////////////////////////
#if (Psep) && ((PbarT) || (PbarP))
	Serial.print('|');
#endif // (Psep) && ((PbarT) || (PbarP))
#if PbarT
	Serial.print(MM_baro[0], 1);
	Serial.print('\t');
#endif // PbarT
#if PbarP
	Serial.print(MM_baro[1], 1);
	Serial.print('\t');
#endif // PbarP

	/////////////////////////////////////////////////////
#if (Psep) && (PapgH)
	Serial.print('|');
#endif // (Psep) && (PapgH)
#if PapgH
	Serial.print(apg.getAltitude());
	Serial.print('\t');
#endif // PapgH

#if (Psep) && (PapgP)
	Serial.print('|');
#endif // (Psep) && (PapgP)
#if PapgP
	Serial.print(apg.getApgPt());
	Serial.print('\t');
	Serial.print(apg.getApgTm());
	Serial.print('\t');
#endif // PapgP

#if (Psep) && ((PapgA) || (PapgS))
	Serial.print('|');
#endif // Psep
#if PapgA
	Serial.print(apg.getAlpha());
	Serial.print('\t');
#endif // PapgA
#if PapgS
	Serial.print(apg.getSigma(), 7);
	Serial.print('\t');
#endif // PapgS


	/////////////////////////////////////////////////////
#if (Psep) && (PapgM)
	Serial.print('|');
#endif // (Psep) && (PapgM)
#if PapgM
	Serial.print(Gutil.getMax(), 5);
	Serial.print('\t');
#endif // PapgM



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
#if PapgW
	if (apg.getApogeu(0.9, 0))
	{
		//#if BuZZ
		//		digitalWrite(buzzPin, 1);
		//#endif // BuZZ
		Serial.print(F("Apogeu: altitude - "));
		Serial.print(apg.getApgPt());
		Serial.print(F(" m, tempo - "));
		Serial.print(apg.getApgTm());
		Serial.print(F(" s\t"));
	}
	if (rec.getSysState())
	{
		if (rec.getP1S(0)) Serial.print(F("Acionamento 1\t"));
		if (rec.getP2S(0)) Serial.print(F("Acionamento 2\t"));
	}
#endif // PapgW


#if (PbarT) || (PbarP) || (PaclX) || (PaclY) || (PaclZ) || (PgirX) || (PgirY) || (PgirZ) || (PmagX) || (PmagY) || (PmagZ) || (PapgW) || (PapgH) || (PapgP) || (PapgA) || (PapgS) || (PapgM) || (Pgps) || (Psep) || (Tcom) || (Lcom)
	Serial.println();
#endif // (PbarT) || (PbarP) || (PaclX) || (PaclY) || (PaclZ) || (PgirX) || (PgirY) || (PgirZ) || (PmagX) || (PmagY) || (PmagZ) || (PapgW) || (PapgH) || (PapgP) || (PapgA) || (PapgS) || (PapgM) || (Pgps) || (Psep) || (Tcom) || (Lcom)


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
			for (int i = 0; i < 3; i++) { SDC.theFile.print(MM_acel[i], 3);	SDC.tab(); }
#endif // ADXL345
#if L3G4200D
			for (int i = 0; i < 3; i++) { SDC.theFile.print(MM_giro[i], 1);	SDC.tab(); }
#endif // L3G4200D
#if HMC5883
			for (int i = 0; i < 3; i++) { SDC.theFile.print(MM_magn[i], 1);	SDC.tab(); }
#endif // HMC5883
#if BMP085
			for (int i = 0; i < 2; i++) { SDC.theFile.print(MM_baro[i]);	SDC.tab(); }
#if ApoGee
			SDC.theFile.print(apg.getAltitude()); SDC.tab();
#endif // ApoGee

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
			//if (apg.getApogeu(0.9, 0))
			if (Gutil.mem)
			{
				if (SDC.util.oneTime())
				{
					SDC.theFile.print(F("Apogeu: altitude - "));
					SDC.theFile.print(apg.getApgPt());
					SDC.theFile.print(F(" m, tempo - "));
					SDC.theFile.print(apg.getApgTm());
					SDC.theFile.print(F(" s"));
					SDC.tab();
				}
				if (rec.getP1S(0)) SDC.theFile.print(F("Acionamento 1\t"));
				if (rec.getP2S(0)) SDC.theFile.print(F("Acionamento 2\t"));
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
			Serial.print(F(" parts of "));
			Serial.print(SYSTEM_n);
			Serial.print(F(" working, waiting...\t"));
#if BMP085
			Serial.print(baro.getTemperature());
			Serial.print(F(" "));
			Serial.write(0xB0);
#endif // BMP085


#if GPSmode
			Serial.print(F("C\tLat: "));
			Serial.print(GpS.getLatitude(), 6);
			Serial.print(F("\tLon: "));
			Serial.print(GpS.getLongitude(), 6);
			Serial.print(F("\t"));
			Serial.print(GpS.getDay());
			Serial.print('/');
			Serial.print(GpS.getMonth());
			Serial.print('\t');
			Serial.print(GpS.getHour());
			Serial.print(':');
			Serial.print(GpS.getMinute());
			Serial.print(':');
			Serial.print(GpS.getSecond());
#endif // GPSmode

			Serial.println();

#endif // PRINT
		}
#if LoRamode
		if (LRutil.eachT(LoRaDelay))
		{
			LoRa.print(sysC);
			LoRa.print(F(" parts of "));
			LoRa.print(SYSTEM_n);
			LoRa.print(F(" working, waiting...\t"));
#if BMP085
			LoRa.print(baro.getTemperature());
			LoRa.print(' ');
			LoRa.write(0xB0);
#endif // BMP085


#if GPSmode
			LoRa.print(F("C\tLat: "));
			LoRa.print(GpS.getLatitude(), 6);
			LoRa.print(F("\tLon: "));
			LoRa.print(GpS.getLongitude(), 6);
			LoRa.print(F("\t"));
			LoRa.print(GpS.getDay());
			LoRa.print('/');
			LoRa.print(GpS.getMonth());
			LoRa.print('\t');
			LoRa.print(GpS.getHour());
			LoRa.print(':');
			LoRa.print(GpS.getMinute());
			LoRa.print(':');
			LoRa.print(GpS.getSecond());
#endif // GPSmode

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
