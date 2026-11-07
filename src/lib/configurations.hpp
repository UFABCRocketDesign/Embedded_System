// configurations.hpp

#pragma once

#ifndef _CONFIGURATIONS_HPP
#define _CONFIGURATIONS_HPP

#define BaudRate 115200

/********************************* Subsystems *********************************/

#define REQUIRE_PRINT	(1)		//Require serial debug output (test builds)
#define REQUIRE_BARO	(1)		//Require barometer
#define REQUIRE_ACCEL	(1)		//Require accelerometer
#define REQUIRE_GYRO	(1)		//Require gyroscope
#define REQUIRE_MAGN	(1)		//Require magnetometer
#define REQUIRE_GNSS	(1)		//Require GNSS (GPS)
#define REQUIRE_LoRa	(1)		//Require telemetry
#define REQUIRE_SD		(1)		//Require SD card
#define REQUIRE_BUZZ	(1)		//Require buzzer
#define REQUIRE_DEPLOY	(1)		//Require parachute deployment
#define REQUIRE_RGB		(0)		//Require RGB LED

/********************************** Hardware **********************************/

/************************* Modules ************************/

/* IMU */
#define USE_GY80 (0)						//Use GY80 module
#define USE_GY91 (0)						//Use GY91 module
#define USE_GY912 (1)						//Use GY912 module

/* GNSS/GPS */
#define USE_NEO_M (REQUIRE_GNSS && 1)		//Use Neo6M/Neo8M/Neo10M GPS module

/* Data storage */
#define USE_SD_SPI (REQUIRE_SD && 1)		//Use SPI SD card module

/* Virtual sensors */
#define USE_VIRTUAL (1 & (USING_MODE == MODE_VIRTUAL))	// Use virtual version of things

/***************** Variants of each module ****************/

/**************** GY80 ****************/
#define USE_BMP085 (REQUIRE_BARO && (USE_GY80 || 0))			//Use BMP085 sensor
#define USE_ADXL345 (REQUIRE_ACCEL && (USE_GY80 || 0))			//Use ADXL345 sensor
#define USE_L3G4200D (REQUIRE_GYRO && (USE_GY80 || 0))			//Use L3G4200D sensor
#define USE_HMC5883 (REQUIRE_MAGN && (USE_GY80 || 0))			//Use HMC5883 sensor

/**************** GY91 ****************/
#define USE_BMP280 (REQUIRE_BARO && (USE_GY91 || 0))			//Use BMP280 sensor
#define USE_MPU9250_ACCEL (REQUIRE_ACCEL && (USE_GY91 || 0))	//Use MPU9250 sensor, accelerometer
#define USE_MPU9250_GYRO (REQUIRE_GYRO && (USE_GY91 || 0))		//Use MPU9250 sensor, gyroscope
#define USE_AK8963 (REQUIRE_MAGN && (USE_GY91 || 0))			//Use AK8963 sensor

/**************** GY912 ***************/
#define USE_BMP388 (REQUIRE_BARO && (USE_GY912 || 0))			//Use BMP388 sensor
#define USE_ICM20948_ACCEL (REQUIRE_ACCEL && (USE_GY912 || 0))	//Use ICM20948 sensor, accelerometer
#define USE_ICM20948_GYRO (REQUIRE_GYRO && (USE_GY912 || 0))	//Use ICM20948 sensor, gyroscope
#define USE_AK09916 (REQUIRE_MAGN && (USE_GY912 || 0))			//Use AK09916 sensor

/**************** LoRa ****************/
#define USE_LoRa_DORJI (REQUIRE_LoRa && defined(BOARD_HAS_LoRa_DORJI))	// Dorji LoRa Module (2019 and before)
#define USE_LoRa_E32 (REQUIRE_LoRa && defined(BOARD_HAS_LoRa_E32))		// E32 LoRa Module (2019 and before)

/***************** RGB ****************/
#define USE_RGB (REQUIRE_RGB && defined(BOARD_HAS_RGB))			//Board RGB LED

/************ Double Board ************/
#define TalkingBoard (0)										//When two boards are connected for redundancy system

/*************** Buzzer ***************/
#define USE_ACT_BUZZER (REQUIRE_BUZZ && 0)						//Active buzzer
#define USE_PSS_BUZZER (REQUIRE_BUZZ && 1)						//Passive buzzer
#define USE_BLINK_BUZZ (REQUIRE_BUZZ && 0)						//LED_BUILTIN instead of a buzzer

/*************** VIRTUAL **************/
#define USE_V_BARO (REQUIRE_BARO && (USE_VIRTUAL && 1))			//Use Virtual Barometer
#define USE_V_ACCEL (REQUIRE_ACCEL && (USE_VIRTUAL && 1))		//Use Virtual Accelerometer
#define USE_V_GYRO (REQUIRE_GYRO && (USE_VIRTUAL && 1))			//Use Virtual Gyroscope
#define USE_V_MAGN (REQUIRE_MAGN && (USE_VIRTUAL && 1))			//Use Virtual Magnetometer
#define USE_V_GNSS  (REQUIRE_GNSS && (USE_VIRTUAL && 1))		//Use Virtual GNSS

/******************************** Capabilities ********************************/
#define USE_PRINT (REQUIRE_PRINT)		// Use serial debug output
#define USE_BARO ((USE_V_BARO) || (USE_BMP085) || (USE_BMP280) || (USE_BMP388))				// Use any Barometer
#define USE_ACCEL ((USE_V_ACCEL) || (USE_ADXL345) || (USE_MPU9250_ACCEL) || (USE_ICM20948_ACCEL))	// Use any Accelerometer
#define USE_GYRO ((USE_V_GYRO) || (USE_L3G4200D) || (USE_MPU9250_GYRO) || (USE_ICM20948_GYRO))		// Use any Gyroscope
#define USE_MAGN ((USE_V_MAGN) || (USE_HMC5883) || (USE_AK8963) || (USE_AK09916))			// Use any Magnetometer
#define USE_GNSS ((USE_NEO_M) || (USE_V_GNSS))	// Use any GNSS
#define USE_LoRa ((USE_LoRa_DORJI) || (USE_LoRa_E32))	// Use any radio
#define USE_STORAGE ((USE_SD_SPI))				// Use any storage
#define USE_BuZZ ((USE_ACT_BUZZER) || (USE_PSS_BUZZER) || (USE_BLINK_BUZZ))	//Buzzer mode

/*********************************** Flight ***********************************/

#define ApoGee (USE_BARO && 1)				//Detection of apogee

#define RBF (0)								//Remove Before Flight
#define WU (ApoGee && 1)					//Wait Until Directives
#define WUF (WU && 1)						//Wait Until Flight
#define WUPS (WU && 1)						//Wait Until Pressure Stabilize

#define AnyDeploy (REQUIRE_DEPLOY && ApoGee && defined(BOARD_HAS_IGN_1) && 1)				//Any Parachute Deployment
#define DualDeploy (AnyDeploy && defined(BOARD_HAS_IGN_3) && 1)			//Dual Parachute Deployment
#define DrogueBackup (AnyDeploy && defined(BOARD_HAS_IGN_2) && DualDeploy && 1)	//Drogue Redundancy mode
#define MainBackup (AnyDeploy && defined(BOARD_HAS_IGN_4) && 1)					//Main Redundancy mode

#define DELAYED_MAIN (AnyDeploy && 1)			//Aways delay main deployment

#define ForceSysC (0)						//Force the working systems counter outside RBF and WUF

/*********************************** Options **********************************/

/************ LoRa Options ************/
#define USE_LoRa_CONTIGUOUS (USE_LoRa && 1)		// Force data columns to always exist
#define USE_LoRa_E32_settable (USE_LoRa_E32 && 1)	// Configure the E32 by software instead of fixing M0/M1
#define USE_LoRa_KEYVALUE (USE_LoRa && 1)	// Send LoRa data Using Key-value pair

/*********** Buzzer Options ***********/
#define MORSE_MSG  (USE_BuZZ && 1)				//Morse beeping
#define BEEPING (USE_BuZZ && 0)					//Buzzer mode

/******************************** Data Analysis *******************************/

#define PbarT (USE_PRINT && USE_BARO && 1)		//Print barometer temperature data
#define PbarP (USE_PRINT && USE_BARO && 1)		//Print barometer pressure data

#define PaclX (USE_PRINT && USE_ACCEL && 1)		//Print accelerometer X axis data
#define PaclY (USE_PRINT && USE_ACCEL && 1)		//Print accelerometer Y axis data
#define PaclZ (USE_PRINT && USE_ACCEL && 1)		//Print accelerometer Z axis data

#define PgirX (USE_PRINT && USE_GYRO && 1)		//Print gyroscope x axis data
#define PgirY (USE_PRINT && USE_GYRO && 1)		//Print gyroscope Y axis data
#define PgirZ (USE_PRINT && USE_GYRO && 1)		//Print gyroscope Z axis data

#define PmagX (USE_PRINT && USE_MAGN && 1)		//Print magnetometer x axis data
#define PmagY (USE_PRINT && USE_MAGN && 1)		//Print magnetometer Y axis data
#define PmagZ (USE_PRINT && USE_MAGN && 1)		//Print magnetometer Z axis data

#define papgI (USE_PRINT && ApoGee && 1)		//Print MonoDeploy.info of every instance
#define PapgW (USE_PRINT && ApoGee && 1)		//Print apogee information when detected
#define PapgH (USE_PRINT && ApoGee && 1)		//Print altimeter data
#define PapgB (USE_PRINT && ApoGee && 1)		//Print altimeter base
#define PapgP (USE_PRINT && ApoGee && 0)		//Print current apogee information
#define PapgA (USE_PRINT && ApoGee && 1)		//Print apogee alpha
#define PapgS (USE_PRINT && ApoGee && 1)		//Print apogee sigma
#define PapgM (USE_PRINT && ApoGee && 0)		//Print apogee sigma max

#define Pgps (USE_PRINT && USE_GNSS && 1)		//Print GPS informations
#define Psep (USE_PRINT && 0)					//Print visual separator

#define Tcom (USE_PRINT && 1)					//Print time counter
#define Lcom (USE_PRINT && 0)					//Print loop counter
#define Ncom (USE_PRINT && 0)					//Print eachN counter
#define Ps_n (USE_PRINT && 1)					//Print SYSTEM_n

#define PWMapg (ApoGee && 1)				//Show the apogee coefficient in a LED

#define PERF_Tcom_print (0)				//Print time counter every 100 iterations (for performance tests)

#define P_ANY (PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print)

/********************************** Data Log **********************************/

#define Sgps (USE_STORAGE && USE_GNSS && 1)		//Log GPS data
#define Sapg (USE_STORAGE && ApoGee && 1)		//Log apogee alpha and sigma
#define Sdpl (USE_STORAGE && AnyDeploy && 1)	//Log parachute deployment state
#define Shea (USE_STORAGE && AnyDeploy && 1)	//Log parachute health (igniter continuity)
#define Semg (USE_STORAGE && AnyDeploy && 1)	//Log emergency state
#define Srst (USE_STORAGE && 1)					//Log reset reason on file header

/* Continuous columns: repeat the last known value on every line.
With 0, the data only shows up at the moment of the event. */
#define Sgps_C (Sgps && 1)					//GPS on every line
#define Sdpl_C (Sdpl && 1)					//Deployments on every line

/*********************************** Derived **********************************/

#define COMmode (USE_PRINT || USE_LoRa)								//Any communication channel exists
#define WIREmode (USE_BARO || USE_ACCEL || USE_GYRO || USE_MAGN)	//Any I2C sensor exists

#define Crst (COMmode && 1)			//Report reset reason on Serial/LoRa

constexpr uint8_t SYSTEM_n = ( 0
	#if USE_STORAGE
	+ 1
	#endif // USE_STORAGE
	#if USE_BARO
	+ 1
	#endif // USE_BARO
	#if USE_ACCEL
	+ 1
	#endif // USE_ACCEL
	#if USE_GYRO
	+ 1
	#endif // USE_GYRO
	#if USE_MAGN
	+ 1
	#endif // USE_MAGN
	#if USE_GNSS
	+ 1
	#endif // USE_GNSS
	#if AnyDeploy
	+ 1
	#endif // AnyDeploy
	#if DualDeploy
	+ 1
	#endif // DualDeploy
	#if DrogueBackup
	+ 1
	#endif // DrogueBackup
	#if MainBackup
	+ 1
	#endif // MainBackup
); //Expected count of systems functioning for flight

/*********************************** Guards ***********************************/

#if REQUIRE_BARO && !USE_BARO
#error: REQUIRE_BARO ligado mas nenhum barometro selecionado
#endif
#if REQUIRE_ACCEL && !USE_ACCEL
#error: REQUIRE_ACCEL ligado mas nenhum acelerometro selecionado
#endif
#if REQUIRE_GYRO && !USE_GYRO
#error: REQUIRE_GYRO ligado mas nenhum giroscopio selecionado
#endif
#if REQUIRE_MAGN && !USE_MAGN
#error: REQUIRE_MAGN ligado mas nenhum magnetometro selecionado
#endif
#if REQUIRE_GNSS && !USE_GNSS
#error: REQUIRE_GNSS ligado mas nenhum GNSS selecionado
#endif
#if REQUIRE_LoRa && !USE_LoRa
#error: REQUIRE_LoRa ligado mas a placa nao tem radio
#endif
#if REQUIRE_SD && !USE_STORAGE
#error: REQUIRE_SD ligado mas nenhum modulo de cartao selecionado
#endif
#if REQUIRE_BUZZ && !USE_BuZZ
#error: REQUIRE_BUZZ ligado mas nenhum tipo de buzzer selecionado
#endif
#if REQUIRE_DEPLOY && !AnyDeploy
#error: REQUIRE_DEPLOY ligado mas nenhum acionamento selecionado
#endif
#if REQUIRE_RGB && !USE_RGB
#error: REQUIRE_RGB ligado mas a placa nao tem LED RGB
#endif
#if USE_PRINT && (USING_MODE == MODE_LANCAMENTO)
#warning USE_PRINT ligado em build de LANCAMENTO - o Serial rouba tempo do loop
#endif
#if RBF && (USING_MODE == MODE_LANCAMENTO)
#warning RBF ligado em build de LANCAMENTO - metodo abandonado, confira o jumper
#endif
#if ForceSysC && (USING_MODE == MODE_LANCAMENTO)
#warning ForceSysC ligado em build de LANCAMENTO - o buzzer vai apitar a contagem de sistemas durante o voo
#endif

#endif // _CONFIGURATIONS_HPP