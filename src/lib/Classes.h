// Classes.h

#pragma once

#ifndef _CLASSES_h
#define _CLASSES_h
#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#define SERVO_MODE 0

// #include "Sens/Sens.h" // Base dos sensores
// #include "TriA/TriA.h" // Base dos sensores de 3 eixos
#include "Baro/Baro.h" // Barometro BMP085
#include "Acel/Acel.h" // Acelerometro ADXL345
#include "Magn/Magn.h" // MagnetometroHMC5883
#include "Giro/Giro.h" // Giroscopio L3G4200D
#include "Helpful/Helpful.h" // Auxiliares de codigo
// #include "RoundArray/RoundArray.h" // Vetor rotativo
#include "MovingAverage/MovingAverage.h" // Media movel simples
#include "SerialFilter/SerialFilter.h" // Media movel em cascata
#include "GyGPS/GyGPS.h" // Auxiliar para GPS
#include "Apogeu/Apogeu.h" // Processamento de altitude e deteccao de apogeu
// #include "DuDeploy/DuDeploy.h" // Acionamento de paraquedas duplo
#include "MonoDeploy/MonoDeploy.h" // Acionamento de paraquedas simples
#include "SDCH/SDCH.h" // Auxiliar para gerenciamento de cartao SD
#include "Term/Term.h" // Termometro LM35

#include "MediaMovel/MediaMovel.h"

/*
class ComProtocol
{
public:
	enum information
	{
		none,
		apgDeteced,	//Apogee Detected
		apgHeightV, //Apogee height registered value
		apgMomentV, //Apogee moment registered value
		FirstDNorm,	//First Deploy Normal
		SecndDNorm,	//Second Deploy Normal
		FirstDBack,	//First Deploy Backup
		SecndDBack,	//Seconf Deploy Backup
		WUFDrctvOf,	//Run out of WUF directive
		SystemBoot	//When System turn on
	};
	ComProtocol(HardwareSerial &S, unsigned long baud);
protected:
	HardwareSerial &Com;
	const unsigned long baudRate;
	information lastInfo = none;
	long lastInteger = 0;
	float lastFloating = 0.0f;
	bool successful = false;
public:
	void begin();
	void inform(information info);
	void informInteger(information info, long rawData = 0);
	void informFloating(information info, float rawData = 0.0f);
	information understand();
	long receiveInteger();
	float receiveFloating();
};
*/
#endif
