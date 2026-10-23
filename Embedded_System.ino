/////////////////////////////////////////////////CHOOSE BOARD/////////////////////////////////////////////////

#pragma region ShieldSelection

#include "src/lib/boards.h"

#if defined(ARDUINO_ARCH_AVR)
	#define USING_BOARD MEGA_OCTA_PTH_MK_I
	// #define USING_BOARD MEGA_STACK_DADOS_ACIONAMENTO_2019
	// #define USING_BOARD MEGA_STACK_DADOS_ACIONAMENTO_2020
#elif defined(ARDUINO_ARCH_ESP32)
	// #define USING_BOARD ESP_ESSENTIALS_2025
	// #define USING_BOARD ESP_ESSENTIALS_2026
	#define USING_BOARD ESP_MAIN_SMD_2026
	// #define USING_BOARD ESP_JOHN_SI_SMD_2026
#else
	#error Nao existem placas para a arquitetura selecionada
#endif

#include "src/lib/pinos.h"

#pragma endregion

//////////////////////////////////////////////////CHOOSE MODE//////////////////////////////////////////////////

#pragma region ModeSelection

#include "src/lib/modes.h"

// #define USING_MODE MODE_LANCAMENTO
// #define USING_MODE MODE_ELEVADOR
// #define USING_MODE MODE_ASPIRADOR
// #define USING_MODE MODE_MANUAL
#define USING_MODE MODE_VIRTUAL

#include "src/lib/pressets.h"

#pragma endregion

/////////////////////////////////////////////////CONFIGURATION/////////////////////////////////////////////////

#pragma region Configurations

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
#define USE_GY912 (0)						//Use GY912 module

/* GNSS/GPS */
#define USE_NEO_M (REQUIRE_GNSS && 0)		//Use Neo6M/Neo8M/Neo10M GPS module

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

// #define PRINT (0)							//Print or not things on Serial
// #define SDCard (1)						//Use SD card
// #define GPSmode (1)						//Use GPS
// #define LoRamode (1)						//Serial mode for transmission on LoRa module

// #define ACT_BUZZER (USE_BuZZ && 0)				//Active buzzer in hardware
// #define PSS_BUZZER (USE_BuZZ && 1)				//Passive buzzer in hardware

// #define BlinkBuzzer (USE_BuZZ && 0)				// Use a LED instead of a buzzer
// #define RGB (defined(BOARD_HAS_RGB) && 0)								//RGB LED board

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

#pragma endregion

/////////////////////////////////////////////////////objects///////////////////////////////////////////////////

#pragma region Declarations

#include "src/lib/Classes.h"

#if USE_VIRTUAL
#include "src/lib/VirtualBridge/VirtualBridge.h"
#endif // USE_VIRTUAL

#if USE_BARO
#if 1 < ((USE_V_BARO) + (USE_BMP085) + (USE_BMP280) + (USE_BMP388))
#error: Múltiplos barômetros definidos
#elif USE_V_BARO
#include "src/lib/VirtualBaro/VirtualBaro.h"	// Barometro Virtual
VirtualBaro baro;
#elif USE_BMP085
#include "src/lib/BMP085/BMP085.h"				// Barometro BMP085
BMP085 baro;									//Barometer object declaration
#elif USE_BMP280
#include "src/lib/BMP280/BMP280.h"				// Barometro BMP280
BMP280 baro;									//Barometer object declaration
#elif USE_BMP388
#include "src/lib/BMP388/BMP388.h"				// Barometro BMP388
BMP388 baro;									//Barometer object declaration
#endif // USE_BMP085 / USE_BMP280
// MovingAverage MM_baro[2]{ (2),(2) };			//Array declaration of the moving average filter objects
float MM_baro[2]{};
bool baroHasData = false;
#endif // USE_BARO

#if ApoGee
#include "src/lib/Apogeu/Apogeu.h" // Processamento de altitude e deteccao de apogeu

Apogeu apg(10, 15, 50);						//Apogee checker object declaration
#define LapsMaxT 5							//Maximum time of delay until emergency state declaration by the delay in sensor response. (seconds)
#define EM_mainN_DELAY 60					// Seconds before forced deployment


#if AnyDeploy
#include "src/lib/MonoDeploy/MonoDeploy.h" // Acionamento de paraquedas simples

const bool MonoDeploy::command = IGN_CMD;

#if DualDeploy
#define EM_drogN_DELAY 10					// Seconds before forced deployment
#endif // DualDeploy

#if DrogueBackup || MainBackup
#define sysDelay 2.5
#endif // DrogueBackup || MainBackup

#if MainBackup
#define EM_mainB_DELAY 65					// Seconds before forced deployment
#endif // MainBackup

#if DELAYED_MAIN
#define sysDelay_main 1.0
#endif // DELAYED_MAIN

#if DrogueBackup
#define EM_drogB_DELAY 15					// Seconds before forced deployment
#endif // DrogueBackup


#define pins_drogN (IGN_1, HEAL_1) /*act1*/
#define pins_drogB (IGN_2, HEAL_2) /*act2*/
#define pins_mainN (IGN_3, HEAL_3) /*act3*/
#define pins_mainB (IGN_4, HEAL_4) /*act4*/


struct Recovery
{
	static MonoDeploy mainN;

#if DualDeploy
	static MonoDeploy drogN;
#endif // DualDeploy

#if MainBackup
	static MonoDeploy mainB;
#endif // MainBackup

#if DrogueBackup
	static MonoDeploy drogB;
#endif // DrogueBackup

	static bool begin()
	{
		bool aux = true;
		aux &= mainN.begin();

#if DualDeploy
		aux &= drogN.begin();
#endif // DualDeploy

#if MainBackup
		aux &= mainB.begin();
#endif // MainBackup

#if DrogueBackup
		aux &= drogB.begin();
#endif // DrogueBackup

		return aux;
	}
	static void emergency(bool state)
	{
		if(state) MonoDeploy::sealApogee(true);
		mainN.emergency(state, EM_mainN_DELAY);

		#if DualDeploy
				drogN.emergency(state, EM_drogN_DELAY);
		#endif // DualDeploy

		#if MainBackup
			mainB.emergency(state, EM_mainB_DELAY);
		#endif // MainBackup

		#if DrogueBackup
				drogB.emergency(state, EM_drogB_DELAY);
		#endif // DrogueBackup

	}
	static bool getGlobalState()
	{
		bool aux = false;
		aux |= mainN.getGlobalState();

#if DualDeploy
		aux |= drogN.getGlobalState();
#endif // DualDeploy

#if MainBackup
		aux |= mainB.getGlobalState();
#endif // MainBackup

#if DrogueBackup
		aux |= drogB.getGlobalState();
#endif // DrogueBackup

		return aux;
	}
	static void refresh()
	{
		mainN.refresh();

#if DualDeploy
		drogN.refresh();
#endif // DualDeploy

#if MainBackup
		mainB.refresh();
#endif // MainBackup

#if DrogueBackup
		drogB.refresh();
#endif // DrogueBackup

	}
	static void resetTimer()
	{
		MonoDeploy::resetTimer();
	}
	static void sealApogee(bool apg)
	{
		MonoDeploy::sealApogee(apg);
	}
	static void putHeight(float H)
	{
		MonoDeploy::putHeight(H);
	}
	static bool getApogee()
	{
		return MonoDeploy::getApogee();
	}
} rec;

MonoDeploy Recovery::mainN pins_mainN;

#if DualDeploy
MonoDeploy Recovery::drogN pins_drogN;
#endif // DualDeploy

#if MainBackup
MonoDeploy Recovery::mainB pins_mainB;
#endif // MainBackup

#if DrogueBackup
MonoDeploy Recovery::drogB pins_drogB;
#endif // DrogueBackup

#else
	#warning Essa compilacao nao realiza disparo de paraquedas
#endif // AnyDeploy

#endif // ApoGee

/*
#if WUF

#if ELEVATOR
#define WUFheigh 5
#else
#define WUFheigh 50
#endif // ELEVATOR

#endif // WUF
*/

/*
#if WUPS

#if ELEVATOR
#define WUPSdelay 3
#else
#define WUPSdelay 10
#endif // ELEVATOR

#endif // WUPS
*/

#if USE_ACCEL
#if 1 < ((USE_V_BARO) + (USE_ADXL345) + (USE_MPU9250_ACCEL) + (USE_ICM20948_ACCEL))
#error: Múltiplos acelerômetros definidos
#elif USE_V_BARO
#include "src/lib/VirtualAccel/VirtualAccel.h"	// Acelerometro Virtual
VirtualAccel accel;
#elif USE_ADXL345
#include "src/lib/ADXL345/ADXL345.h" // Accelerometer ADXL345
ADXL345 accel;									//Accelerometer object declaration
#elif USE_MPU9250_ACCEL
#include "src/lib/MPU9250_ACCEL/MPU9250_ACCEL.h" // Accelerometer MPU9250_ACCEL
MPU9250_ACCEL accel(16);									//Accelerometer object declaration
#elif USE_ICM20948_ACCEL
#include "src/lib/ICM20948_ACCEL/ICM20948_ACCEL.h" // Accelerometer MPU9250_ACCEL
ICM20948_ACCEL accel(16);									//Accelerometer object declaration
#endif // USE_ADXL345 / USE_MPU9250_ACCEL
//MovingAverage MM_accel[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_accel[3]{};
#endif // USE_ACCEL

#if USE_GYRO
#if 1 < ((USE_V_GYRO) + (USE_L3G4200D) + (USE_MPU9250_GYRO) + (USE_ICM20948_GYRO))
#error: Múltiplos giroscópios definidos
#elif USE_V_GYRO
#include "src/lib/VirtualGyro/VirtualGyro.h"	// Gyroscope Virtual
VirtualGyro giro;
#elif USE_L3G4200D
#include "src/lib/L3G4200D/L3G4200D.h" // Gyroscope L3G4200D
L3G4200D giro(2000);							//Gyroscope object declaration
#elif USE_MPU9250_GYRO
#include "src/lib/MPU9250_GYRO/MPU9250_GYRO.h" // Gyroscope MPU9250_GYRO
MPU9250_GYRO giro(2000);							//Gyroscope object declaration
#elif USE_ICM20948_GYRO
#include "src/lib/ICM20948_GYRO/ICM20948_GYRO.h" // Gyroscope MPU9250_GYRO
ICM20948_GYRO giro(2000);							//Gyroscope object declaration
#endif // USE_L3G4200D / USE_MPU9250_GYRO
//MovingAverage MM_giro[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_giro[3]{};
#endif // USE_GYRO

#if USE_MAGN
#if 1 < ((USE_V_MAGN) + (USE_HMC5883) + (USE_AK8963) + (USE_AK09916))
#error: Múltiplos magnetômetros definidos
#elif USE_V_MAGN
#include "src/lib/VirtualMagn/VirtualMagn.h"	// Magnetometro Virtual
VirtualMagn magn;
#elif USE_HMC5883
#include "src/lib/HMC5883/HMC5883.h" // Magnetometer HMC5883
HMC5883 magn;									//Magnetometer object declaration
#elif USE_AK8963
#include "src/lib/AK8963/AK8963.h" // Magnetometer AK8963
AK8963 magn;									//Magnetometer object declaration
#elif USE_AK09916
#include "src/lib/AK09916/AK09916.h" // Magnetometer USE_AK09916
AK09916 magn;									//Magnetometer object declaration
#endif // USE_HMC5883 / USE_AK8963
//MovingAverage MM_magn[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_magn[3]{};
#endif // USE_MAGN

#if USE_STORAGE

#include <SPI.h>
#include <SD.h>
#if ARDUINO_ARCH_ESP32
	SPIClass SPI_SD(FSPI);
#endif // ARDUINO_ARCH_ESP32
#include "src/lib/SDCH/SDCH.h" // Auxiliar para gerenciamento de cartao SD

#if ARDUINO_ARCH_ESP32
SDCH SDC(SD_CS_PIN, CURRENT_MODE_PROJECT_NAME, "txt", SPI_SD);						//Declaration of object to help SD card file management
#else
SDCH SDC(SD_CS_PIN, CURRENT_MODE_PROJECT_NAME);						//Declaration of object to help SD card file management
#endif // ARDUINO_ARCH_ESP32
#endif // USE_STORAGE

// #if USE_GNSS
// #include "src/lib/GyGPS/GyGPS.h" // Auxiliar para GPS
// #ifdef ARDUINO_ARCH_ESP32
// HardwareSerial GpSSerial(1);
// GyGPS GpS(GpSSerial, 0, SERIAL_8N1, RX_GPS_ESP, TX_GPS_ESP);
// #else
// GyGPS GpS(Serial1, 0);
// #endif // ARDUINO_ARCH_ESP32
// #endif // USE_GNSS

#if USE_GNSS
#if 1 < ((USE_V_GNSS) + (USE_NEO_M))
#error: Multiplos GPS definidos
#elif USE_V_GNSS
#include "src/lib/VirtualGPS/VirtualGPS.h"	// GPS Virtual
VirtualGPS GpS;
#elif USE_NEO_M
#include "src/lib/GyGPS/GyGPS.h"			// Auxiliar para GPS
#ifdef ARDUINO_ARCH_ESP32
HardwareSerial GpSSerial(1);
GyGPS GpS(GpSSerial, 0, SERIAL_8N1, RX_GPS_ESP, TX_GPS_ESP);
#else
GyGPS GpS(Serial1, 0);
#endif // ARDUINO_ARCH_ESP32
#endif // USE_V_GNSS / USE_NEO_M
#endif // USE_GNSS


#if USE_LoRa
#if 1 < ((USE_LoRa_DORJI) + (USE_LoRa_E32))
#error: Múltiplos LoRas definidos
#elif USE_LoRa_DORJI
#define LoRaDelay 2.5
#elif USE_LoRa_E32
#define LoRaDelay 2.5
#else
#define LoRaDelay 5
#endif // USE_LoRa_DORJI || USE_LoRa_E32
#ifdef ARDUINO_ARCH_ESP32
HardwareSerial LoRa(2);
#else
HardwareSerial &LoRa(Serial3);
#endif
Helpful LRutil;								//Declaration of helpful object to telemetry system
#define LoRaBaudRate 9600

#if USE_LoRa_E32
#if !defined(M0_LORA_PIN) || !defined(M1_LORA_PIN) || !defined(AUX_LORA_PIN)
#error: Placa selecionada nao utiliza LoRa E32
#endif
#if USE_LoRa_E32_settable

#define FREQUENCY_900
#define LoRa_ADDL 0x2A // 42 decimal (Standardized)
#define LoRa_ADDH 0x00
#define LoRa_CHAN 0x2A // 42 decimal (904 - 862)

#include "LoRa_E32.h"
// #include <EEPROM.h>
#include "src/lib/EEDataRegister/EEMap.h"

// uint16_t LoRaEEAddress = 0x0; // Atualizar este valor no setup
Configuration configLoRa;

// struct LoRaEEConfig{
// 	Configuration configuration;
// 	uint16_t compileHash = 0;
// 	uint16_t checkSum = 0;
// };

EEDataRegister<Configuration> loraReg(eeSlotAddress(EE_SLOT_LORA_CONFIG));
static_assert(EEDataRegister<Configuration>::blockSize() <= EE_SLOT_SIZE,
	"LoRa: bloco de configuracao excede o slot");


LoRa_E32 LoRaConfig(&LoRa, byte(AUX_LORA_PIN), byte(M0_LORA_PIN), byte(M1_LORA_PIN), UART_BPS_RATE(LoRaBaudRate));

// uint16_t calcCheckSum(const Configuration& configuration)
// {
// 	uint16_t sum = 0;
// 	const byte* p = (const byte*)&configuration;
// 	for (size_t i = 0; i < sizeof(Configuration); i++) {
// 		sum += p[i];
// 	}
// 	return sum;
// }

void loadLoRaDefaultConfig()
{
	configLoRa.ADDL = LoRa_ADDL;
	configLoRa.ADDH = LoRa_ADDH;
	configLoRa.CHAN = LoRa_CHAN;

	// configLoRa.OPTION.fec = FEC_0_OFF;
	// configLoRa.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
	// configLoRa.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
	// configLoRa.OPTION.transmissionPower = POWER_17;
	// configLoRa.OPTION.wirelessWakeupTime = WAKE_UP_1250;

	// configLoRa.SPED.airDataRate = AIR_DATA_RATE_011_48;
	// configLoRa.SPED.uartBaudRate = UART_BPS_9600;
	// configLoRa.SPED.uartParity = MODE_00_8N1;
}

bool setLoRaConfig()
{
	ResponseStructContainer c = LoRaConfig.getConfiguration();
#if USE_PRINT
	Serial.println(c.status.getResponseDescription());
	Serial.println(c.status.code);
#endif

	if(c.status.code != E32_SUCCESS) {
		c.close();
		return false;
	}

	// It's important get configuration pointer before all other operation
	Configuration configuration = *(Configuration*)c.data;

	//   printParameters(configuration);
	configuration.ADDL = configLoRa.ADDL;
	configuration.ADDH = configLoRa.ADDH;
	configuration.CHAN = configLoRa.CHAN;

	configuration.OPTION.fec = FEC_1_ON;
	configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
	configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
	configuration.OPTION.transmissionPower = POWER_20;
	configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;

	configuration.SPED.airDataRate = AIR_DATA_RATE_101_192;
	// configuration.SPED.uartBaudRate = configLoRa.SPED.uartBaudRate;
	// configuration.SPED.uartParity = configLoRa.SPED.uartParity;

	// Set configuration changed and set to not hold the configuration
	ResponseStatus rs = LoRaConfig.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
#if USE_PRINT
	Serial.println(rs.getResponseDescription());
	Serial.println(rs.code);
#endif
	//   printParameters(configuration);
	c.close();

	return rs.code == E32_SUCCESS;
}

bool getLoRaConfig()
{
	ResponseStructContainer c = LoRaConfig.getConfiguration();
	// It's important get configuration pointer before all other operation
	if(c.status.code != E32_SUCCESS)
	{
		c.close();
		return false;
	}

	configLoRa = *(Configuration*)c.data;

	c.close();

	return true;
}

// uint16_t compileTimeHash() {
// 	const char* date = __DATE__;
// 	const char* time = __TIME__;
// 	uint16_t hash = 0;
// 	while (*date) {
// 		hash = (hash << 5) - hash + *date++;
// 	}
// 	while (*time) {
// 		hash = (hash << 5) - hash + *time++;
// 	}
// 	return hash;
// }

// void saveLoRaEEConfig(){
// 	LoRaEEConfig loRaEEAux;
// 	loRaEEAux.configuration = configLoRa;
// 	loRaEEAux.compileHash = compileTimeHash();
// 	loRaEEAux.checkSum = calcCheckSum(configLoRa);
// 	EEPROM.put(LoRaEEAddress, loRaEEAux);
// 	#if defined(ARDUINO_ARCH_ESP32)
// 	EEPROM.commit();
// 	#endif // defined(ARDUINO_ARCH_ESP32)
// }
void saveLoRaEEConfig(){
	loraReg.data = configLoRa;
	loraReg.saveIfChanged();
}

// bool loadLoRaEEConfig() {
// 	LoRaEEConfig loRaEEAux;
// 	EEPROM.get(LoRaEEAddress, loRaEEAux);

// 	uint16_t sum = calcCheckSum(loRaEEAux.configuration);

// 	if((sum == loRaEEAux.checkSum) && (loRaEEAux.compileHash == compileTimeHash()) && (loRaEEAux.configuration.HEAD == 0xC0 || loRaEEAux.configuration.HEAD == 0xC2))
// 	{
// 		configLoRa = loRaEEAux.configuration;
// 		return true;
// 	}
// 	return false;
// }
bool loadLoRaEEConfig() {
	if (!loraReg.load()) return false;
	if (!(loraReg.data.HEAD == 0xC0 || loraReg.data.HEAD == 0xC2)) return false;
	configLoRa = loraReg.data;
	return true;
}

#define RX_CHG_FREQ_REQ_HEAD "MUD4R_FR3Q_PFV.CH4N"
							// CHAN
#define RX_CHG_FREQ_REQ_MID "_"
							//	ADDH / ADDL
#define RX_CHG_FREQ_REQ_TAIL "#"

#define TX_CHG_FREQ_CONFIRM_HEAD "CTZ_FR3Q.CH4N"
							//	CHAN
#define TX_CHG_FREQ_CONFIRM_MID "_"
							// ADDH / ADDL
#define TX_CHG_FREQ_CONFIRM_TAIL "#"

#define RX_CHG_FREQ_OK "1SSO_MSM"

#define RX_CHG_FREQ_VRFY "MUD0U_MSM"

#define TX_CHG_FREQ_RESP "JUR0_JUR4D1NH0"

#define RX_CHG_FREQ_FINAL "B04"

#define TX_CHG_FREQ_ERROR "N4N4N1N4N40"

const char chgFreqReqHead[] = RX_CHG_FREQ_REQ_HEAD;
constexpr size_t chgFreqReqHeadLen = sizeof(chgFreqReqHead) - 1;

const char chgFreqReqMid[] = RX_CHG_FREQ_REQ_MID;
constexpr size_t chgFreqReqMidLen = sizeof(chgFreqReqMid) - 1;

const char chgFreqReqTail[] = RX_CHG_FREQ_REQ_TAIL;
constexpr size_t chgFreqReqTailLen = sizeof(chgFreqReqTail) - 1;

constexpr size_t chgFreqReqLen = chgFreqReqHeadLen + 2 + chgFreqReqMidLen + 4 + chgFreqReqTailLen;

const char chgFreqCfmHead[] = TX_CHG_FREQ_CONFIRM_HEAD;
constexpr size_t chgFreqCfmHeadLen = sizeof(chgFreqCfmHead) - 1;

const char chgFreqCfmMid[] = TX_CHG_FREQ_CONFIRM_MID;
constexpr size_t chgFreqCfmMidLen = sizeof(chgFreqCfmMid) - 1;

const char chgFreqCfmTail[] = TX_CHG_FREQ_CONFIRM_TAIL;
constexpr size_t chgFreqCfmTailLen = sizeof(chgFreqCfmTail) - 1;

constexpr size_t chgFreqCfmLen = chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen + 4 + chgFreqCfmTailLen;

const char chgFreqOk[] = RX_CHG_FREQ_OK;
constexpr size_t chgFreqOkLen = sizeof(chgFreqOk) - 1;

const char chgFreqVrfy[] = RX_CHG_FREQ_VRFY;
constexpr size_t chgFreqVrfyLen = sizeof(chgFreqVrfy) - 1;

const char chgFreqFinal[] = RX_CHG_FREQ_FINAL;
constexpr size_t chgFreqFinalLen = sizeof(chgFreqFinal) - 1;

inline uint8_t asciiHex2Nibble(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}

uint8_t nibble2AsciiHex(byte val){
	if (val < 10) return '0' + val;
	return 'A' + (val - 10);
}

uint8_t hexFromCharPair(const char* ptr) {
  return (asciiHex2Nibble(ptr[0]) << 4) | asciiHex2Nibble(ptr[1]);
}

void embedHexByte(char* target, byte val) {
  target[0] = nibble2AsciiHex(val >> 4);
  target[1] = nibble2AsciiHex(val & 0x0F);
}

unsigned long pauseTelemetryUntil = 0;

void cancelLoRaConfig(bool reverter, Configuration &previousConfig)
{
	LoRa.println(TX_CHG_FREQ_ERROR);
#if USE_PRINT
	Serial.println(F("[LORA RX] ERRO: Handshake abortado ou configuracao invalida!"));
#endif

	if(reverter){
		configLoRa = previousConfig;
		setLoRaConfig();
		saveLoRaEEConfig();
	}
}

enum HandshakeState {
	HS_IDLE,
	HS_WAITING_M_PACKET,
	HS_SENDING_CFM,
	HS_WAITING_1SSO,
	HS_WAITING_MUD0U,
	HS_WAITING_B04
};

void updateLoRaFrequency(){
	static HandshakeState hsState = HS_IDLE;
	HandshakeState oldState = hsState;
	static unsigned long stateTimeout = 0;
	static unsigned long startWait = 0;
	static Configuration previousConfig;
	static int CHAN = LoRa_CHAN;
	static int ADDH = LoRa_ADDH;
	static int ADDL = LoRa_ADDL;
	static char recieved[64] = {};
	static char toSend[chgFreqCfmLen + 1] = {};
	static unsigned long sendCfmAt = 0;

	int discardLimit = 256; // Safety limit to prevent infinite loops on serial noise

	switch (hsState) {
		case HS_IDLE: {
			if (LoRa.available() > 0) {
				while (LoRa.available() > 0 && LoRa.peek() != 'M' && discardLimit-- > 0) {
					LoRa.read();
				}
				if (LoRa.available() > 0 && LoRa.peek() == 'M') {
					startWait = millis();
					hsState = HS_WAITING_M_PACKET;
				}
			}
			break;
		}

		case HS_WAITING_M_PACKET: {
			if (LoRa.available() >= chgFreqReqLen) {
				char tempRec[64] = {};
				uint8_t count = LoRa.readBytesUntil('\n', tempRec, chgFreqReqLen);
				tempRec[count] = '\0';

				// Pause telemetry for 6s
				// extern unsigned long pauseTelemetryUntil;
				pauseTelemetryUntil = millis() + 6000;

#if USE_PRINT
				Serial.println(F("\n========== [LORA RX] =========="));
				Serial.print(F("[LORA RX] Mensagem recebida: "));
				Serial.println(tempRec);
#endif

				if (getLoRaConfig()) {
					previousConfig = configLoRa;
				} else {
					previousConfig = configLoRa;
				}

				bool reqCheck = true;
				reqCheck &= (strncmp(tempRec, chgFreqReqHead, chgFreqReqHeadLen) == 0);
				if (reqCheck) {
					reqCheck &= (strncmp(tempRec + chgFreqReqHeadLen + 2, chgFreqReqMid, chgFreqReqMidLen) == 0);
				}
				if (reqCheck) {
					reqCheck &= (strncmp(tempRec + chgFreqReqHeadLen + 2 + chgFreqReqMidLen + 4, chgFreqReqTail, chgFreqReqTailLen) == 0);
				}

				if (!reqCheck) {
#if USE_PRINT
					Serial.println(F("[LORA RX] ERRO: Header/Separador/Tail invalido"));
#endif
					cancelLoRaConfig(false, previousConfig);
					hsState = HS_IDLE;
					break;
				}

				CHAN = hexFromCharPair(tempRec + chgFreqReqHeadLen);
				if (CHAN > 0x45) {
#if USE_PRINT
					Serial.println(F("[LORA RX] ERRO: Canal invalido"));
#endif
					cancelLoRaConfig(false, previousConfig);
					hsState = HS_IDLE;
					break;
				}

				ADDH = hexFromCharPair(tempRec + chgFreqReqHeadLen + 2 + chgFreqReqMidLen);
				ADDL = hexFromCharPair(tempRec + chgFreqReqHeadLen + 2 + chgFreqReqMidLen + 2);

#if USE_PRINT
				Serial.print(F("[LORA RX] Parse OK! CHAN="));
				Serial.print(CHAN, DEC);
				Serial.print(F(" ADDH=0x"));
				Serial.print(ADDH, HEX);
				Serial.print(F(" ADDL=0x"));
				Serial.println(ADDL, HEX);
#endif

				// char toSend[chgFreqCfmLen + 1] = {};
				memset(toSend, 0, sizeof(toSend));
				memcpy(toSend, chgFreqCfmHead, chgFreqCfmHeadLen);
				embedHexByte(toSend + chgFreqCfmHeadLen, CHAN);
				memcpy(toSend + chgFreqCfmHeadLen + 2, chgFreqCfmMid, chgFreqCfmMidLen);
				embedHexByte(toSend + chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen, ADDH);
				embedHexByte(toSend + chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen + 2, ADDL);
				memcpy(toSend + chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen + 4, chgFreqCfmTail, chgFreqCfmTailLen);
				toSend[chgFreqCfmLen] = '\0';

#if USE_PRINT
				Serial.print(F("[LORA RX] Enviando confirmacao: "));
				Serial.println(toSend);
#endif
				sendCfmAt = millis() + 900;
				hsState = HS_SENDING_CFM;

// 				delay(900);
// 				while (LoRa.available() > 0 && discardLimit-- > 0) LoRa.read();
// 				LoRa.println(toSend);

// #if USE_PRINT
// 				Serial.println(F("[LORA RX] Aguardando 1SSO_MSM do GS..."));
// #endif
// 				stateTimeout = millis() + 5000;
// 				hsState = HS_WAITING_1SSO;
			} else if (millis() - startWait >= 100) {
#if USE_PRINT
				Serial.print(F("[LORA RX] Bytes insuficientes ou invalido: "));
				Serial.print(LoRa.available());
				Serial.print(F(" / "));
				Serial.print(chgFreqReqLen);
				Serial.println(F(". Descartando 'M'"));
#endif
				LoRa.read();
				hsState = HS_IDLE;
			}
			break;
		}

		case HS_SENDING_CFM: {
			if(long (millis() - sendCfmAt) < 0) break; // Esperar mais

			while (LoRa.available() > 0 && discardLimit-- > 0) LoRa.read();
			LoRa.println(toSend);

#if USE_PRINT
			Serial.println(F("[LORA RX] Aguardando 1SSO_MSM do GS..."));
#endif
			stateTimeout = millis() + 5000;
			hsState = HS_WAITING_1SSO;
			break;
		}

		case HS_WAITING_1SSO: {
			if (millis() > stateTimeout) {
#if USE_PRINT
				Serial.println(F("[LORA RX] TIMEOUT aguardando 1SSO_MSM"));
#endif
				cancelLoRaConfig(false, previousConfig);
				hsState = HS_IDLE;
				break;
			}

			if (LoRa.available() > 0) {
				while (LoRa.available() > 0 && LoRa.peek() != '1' && discardLimit-- > 0) {
					LoRa.read();
				}
				if (LoRa.available() >= chgFreqOkLen) {
					uint8_t count = LoRa.readBytesUntil('\n', recieved, chgFreqOkLen);
					recieved[count] = '\0';
#if USE_PRINT
					Serial.print(F("[LORA RX] Recebeu: "));
					Serial.println(recieved);
#endif

					if (strncmp(recieved, chgFreqOk, chgFreqOkLen) == 0) {
						configLoRa.ADDL = ADDL;
						configLoRa.ADDH = ADDH;
						configLoRa.CHAN = CHAN;

						if (setLoRaConfig()) {
							saveLoRaEEConfig();
#if USE_PRINT
							Serial.println(F("[LORA RX] Frequencia aplicada. Aguardando MUD0U_MSM do GS..."));
#endif
							stateTimeout = millis() + 5000;
							hsState = HS_WAITING_MUD0U;
						} else {
							cancelLoRaConfig(true, previousConfig);
							hsState = HS_IDLE;
						}
					} else {
#if USE_PRINT
						Serial.println(F("[LORA RX] ERRO: 1SSO_MSM invalido"));
#endif
						cancelLoRaConfig(false, previousConfig);
						hsState = HS_IDLE;
					}
				}
			}
			break;
		}

		case HS_WAITING_MUD0U: {
			if (millis() > stateTimeout) {
#if USE_PRINT
				Serial.println(F("[LORA RX] TIMEOUT aguardando MUD0U_MSM"));
#endif
				cancelLoRaConfig(true, previousConfig);
				hsState = HS_IDLE;
				break;
			}

			if (LoRa.available() > 0) {
				while (LoRa.available() > 0 && LoRa.peek() != 'M' && discardLimit-- > 0) {
					LoRa.read();
				}
				if (LoRa.available() >= chgFreqVrfyLen) {
					uint8_t count = LoRa.readBytesUntil('\n', recieved, chgFreqVrfyLen);
					recieved[count] = '\0';
#if USE_PRINT
					Serial.print(F("[LORA RX] Recebeu: "));
					Serial.println(recieved);
#endif

					if (strncmp(recieved, chgFreqVrfy, chgFreqVrfyLen) == 0) {
#if USE_PRINT
						Serial.println(F("[LORA RX] Enviando JUR0_JUR4D1NH0..."));
#endif
						LoRa.println(TX_CHG_FREQ_RESP);
#if USE_PRINT
						Serial.println(F("[LORA RX] Aguardando B04 do GS..."));
#endif
						stateTimeout = millis() + 5000;
						hsState = HS_WAITING_B04;
					} else {
#if USE_PRINT
						Serial.println(F("[LORA RX] ERRO: MUD0U_MSM invalido"));
#endif
						cancelLoRaConfig(true, previousConfig);
						hsState = HS_IDLE;
					}
				}
			}
			break;
		}

		case HS_WAITING_B04: {
			if (millis() > stateTimeout) {
#if USE_PRINT
				Serial.println(F("[LORA RX] TIMEOUT aguardando B04"));
#endif
				cancelLoRaConfig(true, previousConfig);
				hsState = HS_IDLE;
				break;
			}

			if (LoRa.available() > 0) {
				while (LoRa.available() > 0 && LoRa.peek() != 'B' && discardLimit-- > 0) {
					LoRa.read();
				}
				if (LoRa.available() >= chgFreqFinalLen) {
					uint8_t count = LoRa.readBytesUntil('\n', recieved, chgFreqFinalLen);
					recieved[count] = '\0';
#if USE_PRINT
					Serial.print(F("[LORA RX] Recebeu: "));
					Serial.println(recieved);
#endif

					if (strncmp(recieved, chgFreqFinal, chgFreqFinalLen) == 0) {
#if USE_PRINT
						Serial.println(F("[LORA RX] Mudanca de frequencia concluida com sucesso!"));
#endif
					} else {
#if USE_PRINT
						Serial.println(F("[LORA RX] ERRO: B04 invalido"));
#endif
						cancelLoRaConfig(true, previousConfig);
					}
					hsState = HS_IDLE;
				}
			}
			break;
		}
	}

	if (hsState == HS_IDLE) {
		if (oldState != HS_IDLE) pauseTelemetryUntil = 0;
	} else {
		pauseTelemetryUntil = millis() + 6000;	// Renova enquanto o handshake estiver vivo
	}
}


#endif //USE_LoRa_E32_settable
#endif // USE_LoRa_E32

#if USE_LoRa_KEYVALUE

#define LoRa_KEY_LINE			"L" // Line
#define LoRa_KEY_TIME			"T" // Time
#define LoRa_KEY_LAT			"A" // Latitude
#define LoRa_KEY_LON			"O" // Longitude
#define LoRa_KEY_HOUR			"h" // horas
#define LoRa_KEY_MIN			"n" // minutos
#define LoRa_KEY_PREC			"g" // precisão
#define LoRa_KEY_HEIGTH			"H" // altura atual
#define LoRa_KEY_SD				"s" // SD
#define LoRa_KEY_APG_HEIGHT		"a" // altura Apogeu
#define LoRa_KEY_APG_TIME		"t" // tempo Apogeu
#define LoRa_KEY_MAIN_NORMAL	"M" // Main Normal
#define LoRa_KEY_DROGUE_NORMAL	"D" // Drogue Normal
#define LoRa_KEY_MAIN_BACKUP	"m" // Main Backup
#define LoRa_KEY_DROGUE_BACKUP	"d" // Drogue Backup
#define LoRa_KEY_TEMPERATURE	"c" // Temperatura

#endif // USE_LoRa_KEYVALUE

#endif // USE_LoRa

#if TalkingBoard
ComProtocol Talk(Serial2, 9600);			//Declaration of communication protocol object
#endif // TalkingBoard

#if ((USE_PRINT) || (PERF_Tcom_print))
#endif // ((USE_PRINT) || (PERF_Tcom_print))

Helpful Gutil;								//Declaration of helpful object to general cases

#if RBF
#define RBFpin 2							//Pin that the RBF system is connected
#endif // RBF

#if RBF || WUF || ForceSysC
unsigned short sysC = 0;
#endif // RBF || WUF || ForceSysC


#if USE_BuZZ
#if 1 < ((USE_ACT_BUZZER) + (USE_PSS_BUZZER) + (USE_BLINK_BUZZ))
#error: Multiplos tipos de buzzer definidos
#endif

#if USE_BLINK_BUZZ
#define buzzPin LED_BUILTIN
#define buzzCmd HIGH
#endif // USE_BLINK_BUZZ
#endif // USE_BuZZ

#if MORSE_MSG

// #define MORSE_INTERRUPT (1)				// Use interrupt to avoid beep bugs

#include "src/lib/Morse/Morse.h"

#define ALARM_DELAY 10					// Delay after alarm when all systems working properly

#if USE_ACT_BUZZER
MorseAtvBzz mensageiro(buzzPin, buzzCmd, "~ ");
// MorseAtvBzz mensageiro(buzzPin, buzzCmd, "a 1 - . ~ . ^ . < . # . > . [ . ] . { . }");
#elif USE_PSS_BUZZER
Morse mensageiro(buzzPin, "~ ");
// Morse mensageiro(buzzPin, CURRENT_MODE_PROJECT_NAME);
#endif  // USE_ACT_BUZZER / USE_PSS_BUZZER

#if MORSE_INTERRUPT
MORSE_INTERRPUT_PRESET(mensageiro);
#endif // MORSE_INTERRUPT

Helpful Mutil;
#endif // MORSE_MSG

#if BEEPING
#define holdT .1
Helpful beeper;
#endif // BEEPING

#if COMmode
template <typename T> void transmit(T message);
template <typename T> void transmitln(T message);
template <typename T, typename R> void transmit(T message, R value);
template <typename T, typename R> void transmitln(T message, R value);
#endif // COMmode

#pragma endregion

#if (Srst || Crst) && defined(ARDUINO_ARCH_ESP32)
// Nome do motivo de reinicio. O numero fica no log para scripts;
// o nome, para quem le o arquivo depois do voo.
// https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/misc_system_api.html#_CPPv416esp_reset_reasonv
const __FlashStringHelper* resetReasonName(esp_reset_reason_t reset)
{
	switch (reset) {
		case ESP_RST_POWERON:    return F("POWERON");
		case ESP_RST_EXT:        return F("EXT");
		case ESP_RST_SW:         return F("SW");
		case ESP_RST_PANIC:      return F("PANIC");
		case ESP_RST_INT_WDT:    return F("INT_WDT");
		case ESP_RST_TASK_WDT:   return F("TASK_WDT");
		case ESP_RST_WDT:        return F("WDT");
		case ESP_RST_DEEPSLEEP:  return F("DEEPSLEEP");
		case ESP_RST_BROWNOUT:   return F("BROWNOUT");
		case ESP_RST_SDIO:       return F("SDIO");
		case ESP_RST_USB:        return F("USB");
		case ESP_RST_JTAG:       return F("JTAG");
		case ESP_RST_EFUSE:      return F("EFUSE");
		case ESP_RST_PWR_GLITCH: return F("PWR_GLITCH");
		case ESP_RST_CPU_LOCKUP: return F("CPU_LOCKUP");
		default:                 return F("UNKNOWN");
	}
}
#endif // (Srst || Crst) && defined(ARDUINO_ARCH_ESP32)


/////////////////////////////////////////////////////SETUP/////////////////////////////////////////////////////

#pragma region Setup
void setup()
{
#if defined(ARDUINO_ARCH_ESP32) && (ApoGee || USE_LoRa_E32_settable)
	EEPROM.begin(_EEPROM_SIZE);
#endif // defined(ARDUINO_ARCH_ESP32) && (ApoGee || USE_LoRa_E32_settable)
#if PWMapg
	pinMode(PWMout, OUTPUT);
#endif // PWMapg

#if RBF
	pinMode(RBFpin, INPUT_PULLUP);
#endif // RBF

#if USE_RGB
	pinMode(RGB_rPin, OUTPUT);
	pinMode(RGB_gPin, OUTPUT);
	pinMode(RGB_bPin, OUTPUT);
	digitalWrite(RGB_rPin, !RGB_CMD);
	digitalWrite(RGB_gPin, !RGB_CMD);
	digitalWrite(RGB_bPin, !RGB_CMD);
#endif // USE_RGB

#if AnyDeploy
	rec.begin();
#if DualDeploy
	rec.mainN.setHeightCmd(CURRENT_MODE_P2H_NORMAL);
#endif // DualDeploy
#if DELAYED_MAIN
	rec.mainN.setDelayCmd(sysDelay_main);
#endif // DELAYED_MAIN

#endif // AnyDeploy

// Main
#if MainBackup
#if DualDeploy
	rec.mainB.setHeightCmd(CURRENT_MODE_P2H_BACKUP);
#else
#if DELAYED_MAIN
	rec.mainB.setDelayCmd(sysDelay + sysDelay_main);
#else
	rec.mainB.setDelayCmd(sysDelay);
#endif // DELAYED_MAIN
#endif // !DualDeploy
#endif // MainBackup

// Drogue
#if DrogueBackup
	rec.drogB.setDelayCmd(sysDelay);
#endif // DrogueBackup



#if ((USE_PRINT) || (PERF_Tcom_print) || (USE_VIRTUAL))
	Serial.begin(BaudRate);

	#if defined(ARDUINO_ARCH_ESP32)
	delay(ESP32_WAIT_TIME_MS); // Wait to ESP32 properly start
	#endif // defined(ARDUINO_ARCH_ESP32)

	Serial.println();
#if Ps_n
	Serial.print(F("System number: "));
	Serial.println(SYSTEM_n);
#endif // Ps_n
#endif // ((USE_PRINT) || (PERF_Tcom_print))


#if USE_ACT_BUZZER
	pinMode(buzzPin, OUTPUT);
	digitalWrite(buzzPin, !buzzCmd);
#endif // USE_ACT_BUZZER
#if MORSE_MSG
	mensageiro.setup();

#if MORSE_INTERRUPT
	morseInterruptionSetup();
#endif // MORSE_INTERRUPT

	mensageiro.updateMorse();

#endif // MORSE_MSG
#if BEEPING
	beep();
#endif // BEEPING

#if USE_LoRa

#ifdef ARDUINO_ARCH_ESP32
	LoRa.begin(LoRaBaudRate, SERIAL_8N1, RX_LORA_ESP, TX_LORA_ESP);
#else
	LoRa.begin(LoRaBaudRate);
#endif // ARDUINO_ARCH_ESP32

#if USE_LoRa_E32
#if USE_LoRa_E32_settable

	LoRaConfig.begin();

	// #if (USE_LoRa_E32_settable) && (ApoGee)
	// LoRaEEAddress = apg.getEEAddress() + sizeof(float);
	// if(LoRaEEAddress + sizeof(LoRaEEConfig) >= (_EEPROM_SIZE)) LoRaEEAddress = 0;
	// #endif // (USE_LoRa_E32_settable) && (ApoGee)

	loadLoRaDefaultConfig();
	bool loRaEEValid = loadLoRaEEConfig();
	setLoRaConfig();
#else
	pinMode(M0_LORA_PIN,OUTPUT); digitalWrite(M0_LORA_PIN,LOW);
	pinMode(M1_LORA_PIN,OUTPUT); digitalWrite(M1_LORA_PIN,LOW);
#endif  // USE_LoRa_E32_settable
#endif // USE_LoRa_E32

#endif // USE_LoRa

#if Crst && defined(ARDUINO_ARCH_ESP32)
	transmit(F("\nReset: "));
	transmit(resetReasonName(esp_reset_reason()));
#endif // Crst && defined(ARDUINO_ARCH_ESP32)

#if USE_GNSS
	GpS.begin();
	GpS.util.mem = false; // Auxiliar de primeira leitura
	GpS.util.forT(60);
	if (GpS)
	{
		if (GpS.util.oneTime())
		{
#if COMmode
			transmit(F("\nGPS ok "));
			transmit(GpS.getLatitude(), 6);
			transmit(F(", "));
			transmit(GpS.getLongitude(), 6);
#endif // COMmode
		}
	}
	else
	{
#if COMmode
		transmit(F("\nGPS err, waiting signal"));
#endif // COMmode
	}
#endif // USE_GNSS

#if WIREmode
#if ARDUINO_ARCH_ESP32
	Wire.begin(SDA_I2C_ESP, SCL_I2C_ESP);
	Wire.setClock(400000);
	Wire.setTimeOut(3000);
#else
	Wire.begin();
#endif // ARDUINO_ARCH_ESP32
#endif // WIREmode

#if USE_BARO
	baro.begin();
	if (baro)
	{
#if (!USE_BMP085)
		for (short i = 0; i < 50; i++) baro.readAll(); // Contornar tempo de estabilização do filtro interno
#endif // (!USE_BMP085)
#if ApoGee
		for (short i = 0; i < 100; i++) if (baro) apg.addZero(baro.getPressure());
		apg.fixZero(CURRENT_MODE_Fix0range, CURRENT_MODE_Fix0diff);
#endif // ApoGee
#if COMmode
		transmit(F("\nBaro ok "));
#endif // COMmode

#if PapgB
		if(apg.getFixZero()){
			Serial.print(F("(Using EEPROM Zero Ref @ < 0x"));
			Serial.print(apg.getEEAddress(), HEX);
			Serial.print(F(" >!) "));
		}

		Serial.print(apg.getZero());
		Serial.println();
#elif USE_PRINT
		Serial.println();
#endif // USE_PRINT

#if USE_LoRa
#if ApoGee
		// When system gets a invalid zero height (reset on flight)
		if(apg.getFixZero()) {

			#if USE_LoRa_E32_settable
			LoRa.print(F("(Using EEPROM LoRa settings: ADD < 0x"));
			LoRa.print(configLoRa.ADDH, HEX);
			LoRa.print(configLoRa.ADDL, HEX);
			LoRa.print(F(" > CHAN < 0x"));
			LoRa.print(configLoRa.CHAN, HEX);
			LoRa.print(F(" > Ref @ < 0x "));
			// LoRa.print(LoRaEEAddress, HEX);
			LoRa.print(loraReg.getAddress(), HEX);
			LoRa.print(F(" >)! "));

			if(loRaEEValid)
			{
				LoRa.println(F("Success!"));
			}
			else
			{
				LoRa.println(F("Fail, using default LoRa settings"));
			}
			#endif // USE_LoRa_E32_settable

			LoRa.print(F("(Using EEPROM Zero Ref @ < 0x"));
			LoRa.print(apg.getEEAddress(), HEX);
			LoRa.print(F(" >!) "));
		}
		LoRa.println(apg.getZero());
#endif // ApoGee
		LoRa.println();
#endif // USE_LoRa

	}
	else
	{
#if COMmode
		transmit(F("\nBaro err"));
#endif // COMmode
	}
#endif // USE_BARO

#if USE_ACCEL
	accel.begin();
	if (accel)
	{
#if COMmode
		transmit(F("\nAccel ok"));
#endif // COMmode
	}
	else
	{
#if COMmode
		transmit(F("\nAccel err"));
#endif // COMmode
	}
#endif // USE_ACCEL

#if USE_GYRO
	giro.begin();
	if (giro)
	{
#if COMmode
		transmit(F("\nGiro ok"));
#endif // COMmode
	}
	else
	{
#if COMmode
		transmit(F("\nGiro err"));
#endif // COMmode
	}
#endif // USE_GYRO

#if USE_MAGN
	magn.begin();
	if (magn)
	{
#if COMmode
		transmit(F("\nMagn ok"));
#endif // COMmode
	}
	else
	{
#if COMmode
		transmit(F("\nMagn err"));
#endif // COMmode
	}
#endif // USE_MAGN

#if AnyDeploy && COMmode
	transmit(rec.mainN.info() ? F("\nIgnMainN ok") : F("\nIgnMainN err"));

#if DualDeploy
	transmit(rec.drogN.info() ? F("\nIgnDrogN ok") : F("\nIgnDrogN err"));
#endif // DualDeploy

#if MainBackup
	transmit(rec.mainB.info() ? F("\nIgnMainB ok") : F("\nIgnMainB err"));
#endif // MainBackup

#if DrogueBackup
	transmit(rec.drogB.info() ? F("\nIgnDrogB ok") : F("\nIgnDrogB err"));
#endif // DrogueBackup


#endif // AnyDeploy && COMmode


#if USE_STORAGE

#if ARDUINO_ARCH_ESP32
	SPI_SD.begin(SCK_SD_ESP, MISO_SD_ESP, MOSI_SD_ESP, CS_SD_ESP);
#endif // ARDUINO_ARCH_ESP32
	SDC.begin();
	if (SDC)
	{
#if COMmode
		transmit(F("\nSD start OK "));
		transmit(SDC.getFname());
#endif // COMmode

		//////////////////File Header//////////////////

#if Srst && defined(ARDUINO_ARCH_ESP32)
		SDC.theFile.print(F("Reset reason:\t"));
		SDC.theFile.print(int(esp_reset_reason()));
		SDC.theFile.print(F(" <"));
		SDC.theFile.print(resetReasonName(esp_reset_reason()));
		SDC.theFile.println(F(">"));
#endif // Srst && defined(ARDUINO_ARCH_ESP32)
#if ApoGee
		SDC.theFile.print(F("Liftoff flag:\t"));
		SDC.theFile.print(apg.getLiftoff());
		SDC.theFile.println();
		SDC.theFile.print(F("Start at:\t"));
		SDC.theFile.print(apg.getZero());
		SDC.theFile.print(F("\tm\tZero spread:\t"));
		SDC.theFile.print(apg.getSpread());
		SDC.theFile.print(F("\tm"));
		if(apg.getFixZero()) {
			SDC.theFile.print(F("\t(Using EEPROM Zero Ref @ < 0x"));
			SDC.theFile.print(apg.getEEAddress(), HEX);
			SDC.theFile.print(F(" >!) "));
		}
		SDC.theFile.println();
#endif // ApoGee

		///////////////////////////////////////////////

		SDC.theFile.println(F(
			"tempo.s\t"
#if USE_ACCEL
			"mps2.x.accel\tmps2.y.accel\tmps2.z.accel\t"
#endif // USE_ACCEL
#if USE_GYRO
			"dps.x.gyros\tdps.y.gyros\tdps.z.gyros\t"
#endif // USE_GYRO
#if USE_MAGN
			"uT.x.magn\tuT.y.magn\tuT.z.magn\t"
#endif // USE_MAGN
#if USE_BARO
			"c.baro\tpa.baro\t"
#endif // USE_BARO
#if ApoGee
			"m.h.baro\t"
#endif // ApoGee
#if Sgps
			"lat.GPS\tlon.GPS\tm.h.GPS\tmps.GPS\tsat.GPS\tprec.GPS\tnew.GPS\t"
#endif // Sgps
#if Sapg
			"alpha.apg\tsigma.apg\t"
#endif // Sapg
#if Sdpl_C
			"on.mainN\tfire.mainN\t"
#if DualDeploy
			"on.drogN\tfire.drogN\t"
#endif // DualDeploy
#if MainBackup
			"on.mainB\tfire.mainB\t"
#endif // MainBackup
#if DrogueBackup
			"on.drogB\tfire.drogB\t"
#endif // DrogueBackup
#endif // Sdpl_C

#if Shea
			"ok.mainN\t"
#if DualDeploy
			"ok.drogN\t"
#endif // DualDeploy
#if MainBackup
			"ok.mainB\t"
#endif // MainBackup
#if DrogueBackup
			"ok.drogB\t"
#endif // DrogueBackup
#endif // Shea

#if Semg
			"emrg\t"
#endif // Semg


		));

		///////////////////////////////////////////////
/*
		SDC.theFile.println(F(
			"seg\t"
#if USE_ACCEL
			"X\tY\tZ\t"
#endif // USE_ACCEL
#if USE_GYRO
			"X\tY\tZ\t"
#endif // USE_GYRO
#if USE_MAGN
			"X\tY\tZ\t"
#endif // USE_MAGN
#if USE_BARO
			"C\tPascal\t"
#endif // USE_BARO
#if ApoGee
			"m\t"
#endif // ApoGee
#if USE_GNSS
			"Latitude\tLongitude\tAltitude (m)\tspeed\tSat\tPrec\t"
#endif // USE_GNSS
		));
*/
		///////////////////////////////////////////////

		SDC.close();
	}
	else
	{

#if COMmode
		transmit(F("\nSD err"));
#endif // COMmode
	}
#endif // USE_STORAGE

#if COMmode
		transmitln(F(" "));
#endif // COMmode

#if USE_VIRTUAL
	if(Bridge.count() >= VB_MAX_SENSORS)
	{
		transmitln(F("\nERRO: Tabela de ponte sobrecarregada!"));
	}
#endif // USE_VIRTUAL

	////////////////RBF directive////////////////

#if RBF
	RemoveBefore();
#endif // RBF

	////////////////RBF directive////////////////


#if PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
	Serial.println(F(
#endif // PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
#if Lcom
	"loop\t"
#endif // Lcom

#if PERF_Tcom_print
	"temp\t"
#endif // PERF_Tcom_print

#if Tcom
	"tempo.s\t"
#endif // Tcom


#if PaclX
	"mps2.x.accel\t"
#endif // PaclX
#if PaclY
	"mps2.y.accel\t"
#endif // PaclY
#if PaclZ
	"mps2.z.accel\t"
#endif // PaclZ


#if PgirX
	"dps.x.gyros\t"
#endif // PgirX
#if PgirY
	"dps.y.gyros\t"
#endif // PgirY
#if PgirZ
	"dps.z.gyros\t"
#endif // PgirZ


#if PmagX
	"uT.x.magn\t"
#endif // PmagX
#if PmagY
	"uT.y.magn\t"
#endif // PmagY
#if PmagZ
	"uT.z.magn\t"
#endif // PmagZ


#if PbarT
	"c.baro\t"
#endif // PbarT
#if PbarP
	"pa.baro\t"
#endif // PbarP

#if PapgH
	"m.h.baro\t"
#endif // PapgH
#if PapgP
	"m.max.apg\tt.max.apg\t"
#endif // PapgP
#if PapgA
	"alpha.apg\t"
#endif // PapgA
#if PapgS
	"sigma.apg\t"
#endif // PapgS
#if PapgM
	"max.sigma.apg\t"
#endif // PapgM

#if Pgps
	"Lat.GPS\tLon.GPS\tm.h.GPS\tmps.GPS\tsat.GPS\tprec.GPS\t"
#endif // Pgps

#if PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
));
#endif // PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
/*
#if PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
	Serial.println(F(
#endif // PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
#if Lcom
	"\t"
#endif // Lcom

#if PERF_Tcom_print
	"s\t"
#endif // PERF_Tcom_print

#if Tcom
	"s\t"
#endif // Tcom


#if PaclX
	"X\t"
#endif // PaclX
#if PaclY
	"Y\t"
#endif // PaclY
#if PaclZ
	"Z\t"
#endif // PaclZ


#if PgirX
	"X\t"
#endif // PgirX
#if PgirY
	"Y\t"
#endif // PgirY
#if PgirZ
	"Z\t"
#endif // PgirZ


#if PmagX
	"X\t"
#endif // PmagX
#if PmagY
	"Y\t"
#endif // PmagY
#if PmagZ
	"Z\t"
#endif // PmagZ


#if PbarT
	"C\t"
#endif // PbarT
#if PbarP
	"Pascal\t"
#endif // PbarP

#if PapgH
	"m\t"
#endif // PapgH
#if PapgP
	"Max h\tMax t\t"
#endif // PapgP
#if PapgA
	"Alpha\t"
#endif // PapgA
#if PapgS
	"Sigma\t"
#endif // PapgS
#if PapgM
	"Max S\t"
#endif // PapgM

#if Pgps
	"Latitude\tLongitude\tAltitude\tSpeed (m/s)\tSat\tPrec\t"
#endif // Pgps

#if PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
	));
#endif // PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom || PERF_Tcom_print
*/

#if USE_STORAGE
	SDC.util.begin();
#endif // USE_STORAGE

	////////////////WUF directive////////////////

#if WUF
	WaitUntilFlight(CURRENT_MODE_WUFheigh);
#if COMmode
	transmit(F("LiftOff confirmed"));
#endif // COMmode
#endif // WUF

	////////////////WUF directive////////////////

	Gutil.begin();

#if USE_LoRa
	LRutil.begin();
#endif // USE_LoRa

#if ApoGee
	apg.resetTimer();
#endif // ApoGee
#if AnyDeploy
	rec.resetTimer();
#endif // AnyDeploy
#if ApoGee && WUF
	apg.markLiftoff();	// persiste 'ja decolou neste build' (apos os timers)
#endif // ApoGee && WUF

#if WUPS
	WaitUntilPressureStabilize(CURRENT_MODE_WUPSdelay);
#if COMmode
	transmit(F("Pressure Stabilize timer expired"));
#endif // COMmode
#endif // WUPS

}
#pragma endregion

/////////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////

#pragma region Loop
void loop()
{
#if RBF || WUF || ForceSysC
	sysC = 0;
#endif // RBF || WUF || ForceSysC

	Gutil.counter();
	readEverything();

#if ApoGee
	if(baroHasData) apg.calcHeight(baro.getPressure());
#if AnyDeploy
	rec.emergency(baro.getTimeLapse() > 1000000 * LapsMaxT);
	if (rec.getGlobalState())
	{
		apg.apgSigma();
		apg.apgAlpha();
		rec.sealApogee(apg.getApogeu(0.8f));
		rec.putHeight(apg.getHeight());
		rec.refresh();
#if PapgM
		Gutil.comparer(apg.getSigma());
#endif // PapgM
		if (rec.getApogee()) Gutil.mem = 1;
	}
	else {
#if MORSE_MSG
		if(Mutil.oneTime())
		{
			mensageiro.msgAux = F("| ~ | # | ^ | < | > | [ | ] | { | } | _ | $ | % A ");
			mensageiro.msgAux += String(apg.getApgPt());
			mensageiro.msgAux += F(" m");
			mensageiro.setNextMessage(mensageiro.msgAux);
			mensageiro.msgAux = "";
			#if MORSE_INTERRUPT
			mensageiro.unsetQuiet();
			#endif // MORSE_INTERRUPT
		}
		#if !MORSE_INTERRUPT
		mensageiro.updateMorse();
		#endif // !MORSE_INTERRUPT
#endif // MORSE_MSG
#if BEEPING
		beep(SYSTEM_n); //rec
#endif // BEEPING
	}
#endif // AnyDeploy
#endif // ApoGee
#if PWMapg
	analogWrite(PWMout, (char)(apg.getSigma() * 255));
#endif // PWMapg

#if ((USE_PRINT) || (PERF_Tcom_print))
	SerialSend();
#endif // ((USE_PRINT) || (PERF_Tcom_print))

#if USE_STORAGE
	SDSend();
#endif // USE_STORAGE

#if USE_LoRa
	LoRaSend();
#endif // USE_LoRa

#if MORSE_MSG && ForceSysC && (!MORSE_INTERRUPT)
	mensageiro.updateMorse();
#endif // MORSE_MSG  && ForceSysC && (!MORSE_INTERRUPT)
#if BEEPING && ForceSysC
	beep(sysC);
#elif BEEPING && !WUF && !RBF && !ApoGee
	beep(1);
#endif // BEEPING && !WUF && !RBF && !ApoGee
}
#pragma endregion

#pragma region Modules
//////////////////////////////////////////////////////RBF//////////////////////////////////////////////////////

#if RBF
inline void RemoveBefore()
{
	bool rbf = 0;
	Helpful rbfHelper;
	do
	{
		sysC = 0;
		readEverything();
		rbf = digitalRead(RBFpin);
		if (rbfHelper.oneTime())
		{
#if USE_PRINT
			Serial.print(sysC);
			Serial.print(F(" parts of "));
			Serial.print(SYSTEM_n);
			Serial.print(F(" working, waiting...\t"));
#if USE_BARO
			Serial.print(baro.getTemperature());
			Serial.print(F(" "));
			Serial.write(0xB0);
#endif // USE_BARO
#if USE_GNSS
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
#endif // USE_GNSS

			Serial.println();

#endif // USE_PRINT
		}
		else if (rbfHelper.eachT(2)) rbfHelper.oneTimeReset();
#if USE_LoRa
#if USE_LoRa_E32_settable
		updateLoRaFrequency();
#endif // USE_LoRa_E32_settable
		LoRaSend();
#endif // USE_LoRa

#if BEEPING
		///////////////////////////////////////

		beep(sysC);

		///////////////////////////////////////

#endif // BEEPING

	} while (!rbf);

#if USE_LoRa_E32_settable
	pauseTelemetryUntil = 0;	// Garante telemetria ativa ao entrar em voo
#endif // USE_LoRa_E32_settable

#if BEEPING
	beep();
#endif // BEEPING
}
#endif // RBF

//////////////////////////////////////////////////////WU//////////////////////////////////////////////////////

#if WU
inline void WaitUntil()
{
	sysC = 0;
	#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux = "";
	#endif // MORSE_MSG
			Gutil.counter();
			readEverything();

	#if ApoGee
			if(baroHasData) apg.calcHeight(baro.getPressure());
			apg.apgSigma();
			apg.apgAlpha();
	#if PapgM
			Gutil.comparer(apg.getSigma());
	#endif // PapgM
	#endif // ApoGee
	#if PWMapg
			analogWrite(PWMout, (char)(apg.getSigma() * 255));
	#endif // PWMapg

	#if ((USE_PRINT) || (PERF_Tcom_print))
			SerialSend();
	#endif // ((USE_PRINT) || (PERF_Tcom_print))

	#if USE_STORAGE
			SDSend();
	#endif // USE_STORAGE

	#if USE_LoRa
			LoRaSend();
	#endif // USE_LoRa
}
#endif // WU

#if WUF
inline void WaitUntilFlight(float minHeight)
{
	do
	{
		#if USE_LoRa_E32_settable
		updateLoRaFrequency();
		#endif // USE_LoRa_E32_settable

		WaitUntil();

#if MORSE_MSG
		// if(Mutil.oneTime()) if(mensageiro.msgAux.length() > 0) mensageiro.setNextMessage("# "+mensageiro.msgAux);
		// if(mensageiro.updateMorse()) Mutil.oneTimeReset();

		if(mensageiro.msgAux.length() > 0) { // Any system failed
			if(Mutil.oneTime()) mensageiro.setNextMessage("# "+mensageiro.msgAux);
			#if MORSE_INTERRUPT
			mensageiro.unsetQuiet();
			#else
			mensageiro.updateMorse();
			#endif // !MORSE_INTERRUPT
			if(mensageiro.getMessageComplete()) Mutil.oneTimeReset();
		} else { // No system failed
			if(!mensageiro.getQuiet())
			{
				#if !MORSE_INTERRUPT
				mensageiro.updateMorse();
				#endif // !MORSE_INTERRUPT
				if(mensageiro.getMessageComplete()) // After sound sequence completed
				{
					mensageiro.setQuiet(); // Mute sound
					Mutil.forT(ALARM_DELAY); // Wait some time
				}
			}
			else {
				if(!Mutil.forT()) {
					#if MORSE_INTERRUPT
					mensageiro.unsetQuiet();
					#else
					mensageiro.updateMorse(); // Restart sound
					#endif // MORSE_INTERRUPT
				}
			}
		}
#endif // MORSE_MSG
#if BEEPING
		beep(sysC);
#endif // BEEPING

	}
	while (abs(apg.getHeight()) < minHeight && !(baro.getTimeLapse() > 1000000 * LapsMaxT));

#if USE_LoRa_E32_settable
	pauseTelemetryUntil = 0;	// Garante telemetria ativa ao entrar em voo
#endif // USE_LoRa_E32_settable

#if MORSE_MSG
	mensageiro.setQuiet();
	Mutil.oneTimeReset();
#endif // MORSE_MSG
#if BEEPING
	beep();
#endif // BEEPING
}
#endif // WUF

#if WUPS
inline void WaitUntilPressureStabilize(float waitTime) {
	Gutil.forT(waitTime);
	while (Gutil.forT())
	{
		WaitUntil();
	}
	// Ensure that apogee point was not given during pressure anomaly
	apg.resetAptPt();
}
#endif // WUPS

//////////////////////////////////////////////////////SPT//////////////////////////////////////////////////////

#if ((USE_PRINT) || (PERF_Tcom_print))
inline void SerialSend()
{

#if Ncom
	if (Gutil.eachN(100))
	{
		Serial.print(Gutil.getCount());
		Serial.print('\t');
		float T = Gutil.lapse();
		Serial.print(T, 5);
		Serial.print('\t');
		Serial.println(1 / T, 5);
	}
	else
	{
		Gutil.lapse();
	}
#endif // Ncom

#if Lcom
	Serial.print(Gutil.getCount());
	Serial.print(F(":\t"));
#endif // Lcom

#if PERF_Tcom_print // Print elapsed iteration time every 100 iterations, for performance tests only
	static Helpful G;
	static float a;

	float b= Gutil.sinceBegin();

	//Serial.print((b-a)*500, 3);
	if ((b - a) * 100 > 1) Serial.println(G.lapse(),6);
	else G.lapse();
	a = b;
#endif // PERF_Tcom_print

#if Tcom
	Serial.print(Gutil.sinceBegin());
	Serial.print('\t');
#endif // Tcom

	////////////////////////////////////////////////////
#if Psep && (PaclX || PaclY || PaclZ)
	Serial.print('|');
#endif // Psep && (PaclX || PaclY || PaclZ)
#if PaclX
	Serial.print(MM_accel[0], 3);
	Serial.print('\t');
#endif // PaclX

#if PaclY
	Serial.print(MM_accel[1], 3);
	Serial.print('\t');
#endif // PaclY

#if PaclZ
	Serial.print(MM_accel[2], 3);
	Serial.print('\t');
#endif // PaclZ

	/////////////////////////////////////////////////////
#if Psep && (PgirX || PgirY || PgirZ)
	Serial.print('|');
#endif // Psep && (PgirX || PgirY || PgirZ)
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
#if Psep && (PmagX || PmagY || PmagZ)
	Serial.print('|');
#endif // Psep && (PmagX || PmagY || PmagZ)
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
#if Psep && (PbarT || PbarP)
	Serial.print('|');
#endif // Psep && (PbarT || PbarP)
#if PbarT
	Serial.print(MM_baro[0], 1);
	Serial.print('\t');
#endif // PbarT
#if PbarP
	Serial.print(MM_baro[1], 1);
	Serial.print('\t');
#endif // PbarP

	/////////////////////////////////////////////////////
#if Psep && PapgH
	Serial.print('|');
#endif // Psep && PapgH
#if PapgH
	Serial.print(apg.getHeight());
	Serial.print('\t');
#endif // PapgH

#if Psep && PapgP
	Serial.print('|');
#endif // Psep && PapgP
#if PapgP
	Serial.print(apg.getApgPt());
	Serial.print('\t');
	Serial.print(apg.getApgTm());
	Serial.print('\t');
#endif // PapgP

#if Psep && (PapgA || PapgS)
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
#if Psep && PapgM
	Serial.print('|');
#endif // Psep && PapgM
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
	Serial.print(GpS.getChars());
	Serial.print('\t');
	Serial.print(GpS.getYear());
	Serial.print('-');
	Serial.print(GpS.getMonth());
	Serial.print('-');
	Serial.print(GpS.getDay());
	Serial.print('\t');
	Serial.print(GpS.getHour());
	Serial.print(':');
	Serial.print(GpS.getMinute());
	Serial.print(':');
	Serial.print(GpS.getSecond());
	Serial.print('\t');
	Serial.print(GpS.getKph());
	Serial.print('\t');
	Serial.print(GpS.getMps());
	Serial.print('\t');
#endif // Pgps
#if PapgW
	if (Gutil.mem)
	{
		Serial.print(F("H: "));
		Serial.print(apg.getApgPt());
		Serial.print(F(" m\tT:"));
		Serial.print(apg.getApgTm());
		Serial.print(F(" s\t"));
	}
#if COMmode
#if AnyDeploy
	if (rec.getGlobalState())
	{
		if (rec.mainN.getState(0))
		{
#if USE_STORAGE
			rec.mainN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act MainN:"));
			Serial.print(rec.mainN.getDeploymentHeight());
			Serial.print(F("m\t"));
		}

#if DualDeploy
		if (rec.drogN.getState(0))
		{
#if USE_STORAGE
			rec.drogN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act DrogueN:"));
			Serial.print(rec.drogN.getDeploymentHeight());
			Serial.print(F("m\t"));
		}
#endif // DualDeploy

#if MainBackup
		if (rec.mainB.getState(0))
		{
#if USE_STORAGE
			rec.mainB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act MainB:"));
			Serial.print(rec.mainB.getDeploymentHeight());
			Serial.print(F("m\t"));
		}
#endif // MainBackup

#if DrogueBackup
		if (rec.drogB.getState(0))
		{
#if USE_STORAGE
			rec.drogB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act DrogueB:"));
			Serial.print(rec.drogB.getDeploymentHeight());
			Serial.print(F("m\t"));
		}

#endif // DrogueBackup
	}
#endif // AnyDeploy
#endif // COMmode

#endif // PapgW

#if PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom
	Serial.println();
#endif // PbarT || PbarP || PaclX || PaclY || PaclZ || PgirX || PgirY || PgirZ || PmagX || PmagY || PmagZ || PapgW || PapgH || PapgP || PapgA || PapgS || PapgM || Pgps || Psep || Tcom || Lcom

}
#endif // ((USE_PRINT) || (PERF_Tcom_print))

//////////////////////////////////////////////////////SDC//////////////////////////////////////////////////////

#if USE_STORAGE
inline void SDSend()
{
	if (!SDC.util.mem)
	{
		if (SDC)
		{
			SDC.theFile.print(SDC.util.sinceBegin(), 3); SDC.tab();
#if USE_ACCEL
			for (short i = 0; i < 3; i++) { SDC.theFile.print(MM_accel[i], 3);	SDC.tab(); }
#endif // USE_ACCEL
#if USE_GYRO
			for (short i = 0; i < 3; i++) { SDC.theFile.print(MM_giro[i], 1);	SDC.tab(); }
#endif // USE_GYRO
#if USE_MAGN
			for (short i = 0; i < 3; i++) { SDC.theFile.print(MM_magn[i], 1);	SDC.tab(); }
#endif // USE_MAGN
#if USE_BARO
			for (short i = 0; i < 2; i++) { SDC.theFile.print(MM_baro[i]);		SDC.tab(); }
#if ApoGee
			SDC.theFile.print(apg.getHeight()); SDC.tab();
#endif // ApoGee

#endif // USE_BARO

#if Sgps
#if (!Sgps_C)
			if (GpS.isNew())
			{
#endif // (!Sgps_C)
				SDC.theFile.print(GpS.getLatitude(), 6);	SDC.tab(); //Latitude
				SDC.theFile.print(GpS.getLongitude(), 6);	SDC.tab(); //Longitude
				SDC.theFile.print(GpS.getAltitude());		SDC.tab(); //Altitude
				SDC.theFile.print(GpS.getMps());			SDC.tab(); //Velocidade
				SDC.theFile.print(GpS.getSatellites());		SDC.tab(); //Numero de satelites
				SDC.theFile.print(GpS.getPrecision());		SDC.tab(); //Precisao
#if (!Sgps_C)
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
#endif // (!Sgps_C)
			SDC.theFile.print(GpS.isNew());				SDC.tab(); //Fix novo nesta linha
#endif // Sgps

#if Sapg
			SDC.theFile.print(apg.getAlpha());				SDC.tab(); //Apogeu declarado
			SDC.theFile.print(apg.getSigma(), 3);			SDC.tab(); //Confianca do apogeu
#endif // Sapg
#if Sdpl_C
			SDC.theFile.print(rec.mainN.getGlobalState(0));	SDC.tab(); //Main normal ja acionou
			SDC.theFile.print(rec.mainN.getState(1));		SDC.tab(); //Main normal disparando agora
#if DualDeploy
			SDC.theFile.print(rec.drogN.getGlobalState(0));	SDC.tab();
			SDC.theFile.print(rec.drogN.getState(1));		SDC.tab();
#endif // DualDeploy
#if MainBackup
			SDC.theFile.print(rec.mainB.getGlobalState(0));	SDC.tab();
			SDC.theFile.print(rec.mainB.getState(1));		SDC.tab();
#endif // MainBackup
#if DrogueBackup
			SDC.theFile.print(rec.drogB.getGlobalState(0));	SDC.tab();
			SDC.theFile.print(rec.drogB.getState(1));		SDC.tab();
#endif // DrogueBackup
#endif // Sdpl_C

#if Shea
			SDC.theFile.print(rec.mainN.info());			SDC.tab(); //Continuidade main normal
#if DualDeploy
			SDC.theFile.print(rec.drogN.info());			SDC.tab();
#endif // DualDeploy
#if MainBackup
			SDC.theFile.print(rec.mainB.info());			SDC.tab();
#endif // MainBackup
#if DrogueBackup
			SDC.theFile.print(rec.drogB.info());			SDC.tab();
#endif // DrogueBackup
#endif // Shea

#if Semg
			SDC.theFile.print(rec.mainN.getEmergency());	SDC.tab(); //Estado de emergencia
#endif // Semg

#if ApoGee
			//if (apg.getApogeu(0.9, 0))
			if (Gutil.mem)
			{
				if (SDC.util.oneTime())
				{
					SDC.theFile.print(F("H:"));
					SDC.theFile.print(apg.getApgPt());
					SDC.theFile.print(F(" m\tT:"));
					SDC.theFile.print(apg.getApgTm());
					SDC.theFile.print(F(" s"));
					SDC.tab();
				}
#if AnyDeploy
				if (rec.mainN.getState(0))
				{
#if USE_LoRa
					rec.mainN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act MainN:"));
					SDC.theFile.print(rec.mainN.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}

#if DualDeploy
				if (rec.drogN.getState(0))
				{
#if USE_LoRa
					rec.drogN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act DrogueN:"));
					SDC.theFile.print(rec.drogN.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}
#endif // DualDeploy

#if MainBackup
				if (rec.mainB.getState(0))
				{
#if USE_LoRa
					rec.mainB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act MainB:"));
					SDC.theFile.print(rec.mainB.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}
#endif // MainBackup

#if DrogueBackup
				if (rec.drogB.getState(0))
				{
#if USE_LoRa
					rec.drogB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act DrogueB:"));
					SDC.theFile.print(rec.drogB.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}
#endif // DrogueBackup


#endif // AnyDeploy

			}
#endif // ApoGee
			SDC.theFile.println();
			SDC.close();
#if RBF || WUF || ForceSysC
			sysC++;
#endif // RBF || WUF || ForceSysC

		}
		else
		{
			SDC.util.mem = 1;
#if MORSE_MSG
			if(!mensageiro.getQuiet()) mensageiro.msgAux += " S"; // ...
#endif  // MORSE_MSG
		}
	}
	else{
		if (SDC.util.eachT(15)) if (SDC.begin()) SDC.util.mem = 0;
		#if MORSE_MSG
			if(!mensageiro.getQuiet()) mensageiro.msgAux += " S"; // ...
		#endif  // MORSE_MSG
	}
}
#endif // USE_STORAGE

//////////////////////////////////////////////////////BZZ//////////////////////////////////////////////////////

#if BEEPING
inline void beep(unsigned int N)
{
	if (beeper.eachT(holdT*SYSTEM_n * 4) || beeper.oneTime())
	{
		beeper.mem = buzzCmd;
		beeper.counterReset();
		beeper.forT(holdT);
	}
	if (beeper.getCount() < (N + 1) * 2) if (!beeper.forT())
	{
		digitalWrite(buzzPin, beeper.mem);
		beeper.mem = !beeper.mem;
		beeper.counter();
		beeper.forT(holdT);
	}
}

inline void beep()
{
	digitalWrite(buzzPin, !buzzCmd);
	beeper.counterReset();
}
#endif // BEEPING

//////////////////////////////////////////////////////LRM//////////////////////////////////////////////////////

#if USE_LoRa
inline void LoRaSend()
{
#if USE_LoRa_E32_settable
	if (millis() < pauseTelemetryUntil) return;
#endif // USE_LoRa_E32_settable

	/*
	L - Line
	T - Time
	A - Latitude
	O - Longitude
	h - horas
	n - minutos
	g - precisão
	H - altura atual
	s - SD
	a - altura Apogeu
	t - tempo Apogeu
	M - Main Normal
	D - Drogue Normal
	m - Main Backup
	d - Drogue Backup
	c - Temperatura
	*/
	if (LRutil.eachT(LoRaDelay))
	{
		LoRa.println();
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_LINE)); // >>L<<ine
#endif // USE_LoRa_KEYVALUE
		LoRa.print(LRutil.counter());
		LoRa.print('\t');
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_TIME)); // >>T<<ime
#endif // USE_LoRa_KEYVALUE
		LoRa.print(LRutil.sinceBegin());
		LoRa.print('\t');
#if USE_GNSS
		if(GpS.util.mem) { // Somente mandar dados validos apos primeira leitura bem sucedida
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_LAT)); // l>A<titude
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getLatitude(), 6);//Latitude
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_LON)); // l>O<ngitude
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getLongitude(), 6);//Longitude
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_HOUR)); // >>h<<ora
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getHour());//Hora
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_MIN)); // mi>>n<<uto
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getMinute());//Minuto
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_PREC)); // Presicão >>g<<
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getPrecision());//Precisao
			LoRa.print('\t');
		} else {
			LoRa.print(F(
#if USE_LoRa_KEYVALUE
				LoRa_KEY_LAT
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_LON
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_HOUR
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_MIN
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_PREC
#endif // USE_LoRa_KEYVALUE
				"~\t"
			));
		}
#endif // USE_GNSS
#if ApoGee
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_HEIGTH)); // >>H<<eight
#endif // USE_LoRa_KEYVALUE
		LoRa.print(apg.getHeight());
		LoRa.print('\t');
		// LoRa.print(apg.getSigma(), 3);
		// LoRa.print('\t');
#endif // ApoGee
#if USE_STORAGE
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_SD)); // >>s<<D
#endif // USE_LoRa_KEYVALUE
		LoRa.print(!SDC.util.mem);
		LoRa.print('\t');
#endif // USE_STORAGE
#if ApoGee
		if (Gutil.mem)
		{
			LoRa.print(F(LoRa_KEY_APG_HEIGHT)); // >>a<<pogee height
			LoRa.print(apg.getApgPt());
			LoRa.print(F("\t" LoRa_KEY_APG_TIME)); // apogee >>t<<ime
			LoRa.print(apg.getApgTm());
			LoRa.print('\t');
		}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_APG_HEIGHT
#endif // USE_LoRa_KEYVALUE
		"~\t"
#if USE_LoRa_KEYVALUE
		LoRa_KEY_APG_TIME
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // ApoGee
		//LRutil.oneTimeReset();

#if AnyDeploy
	if (
#if USE_LoRa_CONTIGUOUS
		rec.mainN.getGlobalState(1)
#else
		rec.mainN.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
		// rec.mainN.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_MAIN_NORMAL)); // >>M<<ain normal
		LoRa.print(rec.mainN.getDeploymentHeight());
#else
		LoRa.print(F("Act MainN:"));
		LoRa.print(rec.mainN.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_APG_HEIGHT
#endif // FEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS

#if DualDeploy
	if (
#if USE_LoRa_CONTIGUOUS
		rec.drogN.getGlobalState(1)
#else
		rec.drogN.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_DROGUE_NORMAL)); // >>D<<rogue normal
		LoRa.print(rec.drogN.getDeploymentHeight());
#else
		// rec.drogN.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
		LoRa.print(F("Act DrogueN:"));
		LoRa.print(rec.drogN.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_DROGUE_NORMAL
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // DualDeploy

#if MainBackup
	if (
#if USE_LoRa_CONTIGUOUS
		rec.mainB.getGlobalState(1)
#else
		rec.mainB.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_MAIN_BACKUP)); // >>m<<ain backup
		LoRa.print(rec.mainB.getDeploymentHeight());
#else
	// rec.mainB.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
		LoRa.print(F("Act MainB:"));
		LoRa.print(rec.mainB.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_MAIN_BACKUP
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // MainBackup

#if DrogueBackup
	if (
#if USE_LoRa_CONTIGUOUS
		rec.drogB.getGlobalState(1)
#else
		rec.drogB.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_DROGUE_BACKUP)); // >>d<<rogue Backup
		LoRa.print(rec.drogB.getDeploymentHeight());
#else
		// rec.drogB.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
		LoRa.print(F("Act DrogueB:"));
		LoRa.print(rec.drogB.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_DROGUE_BACKUP
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // DrogueBackup

#endif // AnyDeploy
#if USE_BARO
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_TEMPERATURE)); // >>c<< temperatura
#endif // USE_LoRa_KEYVALUE
		LoRa.print(baro.getTemperature());
		LoRa.print('\t');
#endif // USE_BARO
  }
}
#endif // USE_LoRa

//////////////////////////////////////////////////////COM//////////////////////////////////////////////////////

#if COMmode
template <typename T> void transmit(T message)
{
#if USE_PRINT
	Serial.print(message);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.print(message);
#endif // USE_LoRa
}

template <typename T> void transmitln(T message)
{
#if USE_PRINT
	Serial.println(message);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.println(message);
#endif // USE_LoRa
}

template <typename T, typename R> void transmit(T message, R value)
{
#if USE_PRINT
	Serial.print(message, value);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.print(message, value);
#endif // USE_LoRa
}

template <typename T, typename R> void transmitln(T message, R value)
{
#if USE_PRINT
	Serial.println(message, value);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.println(message, value);
#endif // USE_LoRa
}

#endif // COMmode

//////////////////////////////////////////////////////RET//////////////////////////////////////////////////////

inline void readEverything()
{
#if USE_VIRTUAL
	Bridge.update();
#endif // USE_VIRTUAL
#if USE_BARO
	if (baro)
	{
		MM_baro[0] = baro.getTemperature();
		MM_baro[1] = baro.getPressure();
#if RBF || WUF || ForceSysC
		sysC++;
#endif // RBF || WUF || ForceSysC
		baroHasData = true;
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " B"; // -...
		baroHasData = false;
#endif  // MORSE_MSG
	}
#endif // USE_BARO
#if USE_ACCEL
	if (accel)
	{
		MM_accel[0] = accel.getX();
		MM_accel[1] = accel.getY();
		MM_accel[2] = accel.getZ();
#if RBF || WUF || ForceSysC
		sysC++;
#endif // RBF || WUF || ForceSysC
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " A"; // .-
#endif  // MORSE_MSG
	}
#endif // USE_ACCEL
#if USE_GYRO
	if (giro)
	{
		MM_giro[0] = giro.getX();
		MM_giro[1] = giro.getY();
		MM_giro[2] = giro.getZ();
#if RBF || WUF || ForceSysC
		sysC++;
#endif // RBF || WUF || ForceSysC
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " G"; // --.
#endif  // MORSE_MSG
	}
#endif // USE_GYRO
#if USE_MAGN
	if (magn)
	{
		MM_magn[0] = magn.getX();
		MM_magn[1] = magn.getY();
		MM_magn[2] = magn.getZ();
#if RBF || WUF || ForceSysC
		sysC++;
#endif // RBF || WUF || ForceSysC
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " M"; // --
#endif  // MORSE_MSG
	}
#endif // USE_MAGN
#if USE_GNSS
	if (GpS) GpS.util.forT(10);
	if (!GpS.util.mem) if (GpS.isNew()) GpS.util.mem = true; // Auxiliar de primeira leitura bem sucedida
	if (GpS.util.forT()) {
#if RBF || WUF || ForceSysC
		sysC++;
#endif // RBF || WUF || ForceSysC
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " L"; // .-..
#endif  // MORSE_MSG
}

#endif // USE_GNSS
#if AnyDeploy && (RBF || WUF)
	if (rec.mainN.info()) {
		sysC++;
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rmn"; // .-. -- -.
#endif  // MORSE_MSG
	}

#if DualDeploy
	if (rec.drogN.info()) {
		sysC++;
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rdn"; // .-. -.. -.
#endif  // MORSE_MSG
	}
#endif // DualDeploy

#if MainBackup
	if (rec.mainB.info()) {
		sysC++;
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rmb"; // .-. -- -...
#endif  // MORSE_MSG
	}
#endif // MainBackup

#if DrogueBackup
	if (rec.drogB.info()) {
		sysC++;
	} else {
#if MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rdb"; // .-. -.. -...
#endif  // MORSE_MSG
	}
#endif // DrogueBackup

#endif // AnyDeploy && (RBF || WUF)
}
#pragma endregion

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
/*
 *                                                                           I8$$$????$?????????$??????$$?8+
 *                                                                    $$$$?????????????????????????????????????$?$$
 *                                                              +?$???????????????????????????????????????????????????$$:
 *                                                          ?????????????????????$?$??$O???????$$$$????$?????????????????????
 *                                                      $$????????????????$$?,           ?????$$           ,$$$??????????????$$$I
 *                                                  ,$8?????????????$$,                  ?????$$                   $$$????????????$?
 *                                                $?????????????8                        ?????$$                        $?????????????$
 *                                             8??$????????$$                            ?????$$                            $$?????????$??
 *                                          +8??????????$                                ?????$$                               :$?????????I$,
 *                                        $8?????????$                                   ??????$                                   $???????$?$?
 *                                      $?????????$                              I$8$$????????$$????8$?+                             ,$????????$$
 *                                    $????????$8                          88???????????????????????????????88:                         $$????????8
 *                                  $???????$?,                      I$$???????????????????????$$$$????????????????+                      =?$????????
 *                                $?????$$?$                     $???????????????$?????????????,  $????????????????????$                     ?$$??????$
 *                              8$???????$                   ?$????????$$???????????????    ???   ?$I$$???????????????????8+                   $??????$$?
 *                            =$$??????$                  $??????$$=      $???     $?$   $?  I?   ?   $?$I   $???????$$??$?$?$I                  $$?????$?,
 *                           $???????8                 I$???????$I  =8$,  $??  $?   $$  ???????      $??   8I  8?$$   ?????????$?~                 8????????
 *                         $$$??????                 8$???????????   8,  :$?$  ??$  $$   $?  ~?   $   $$       ~?     $???????????I8                 ????????I
 *                        $???????:               ?$??????????????:        ??       ??$I    8?$  I?I  I$  8?$8?$?8  ~ ???????$:8????$$~               +$?$????8
 *                      ~??????$$               $????$8+ 8????????8   ?$    I$$??$$$????????????????????$~    =?$   $????????I   ~$????$+               8??????$
 *                     $?????????$+           $$?????     $?$???????  ????$????????????$?????????$??????????????$   $???????:   ,  :$?????+            $?????????$
 *                    $????????????$        =$???$?$  $$???$$??????$ ?$?????$????$$$$I,           :?$$$????$?????$? 8??????~   ??8  +??????$,        8????????????$
 *                   ??????????????$?8     ?$$?????   $?$??  $?????$????????$I                             I$??????$$$????,   $?$   +????????$     =$???????????????
 *                 ,$?????$  $?????????  8??????????   $??8  $?????????$$=                                     I8????????8   ?$??   $?????????$?  8$????????+~$??????
 *               ?$?????$     $??????????$??    $??        ,$??????$?                                             $???????$       ?$$$8????????$$????????8    ???????,
 *               ???????$       ???????????I     ?8$?8     $$???$?,                                                   +$?????8$,:$?8      $??????????????      ,???????
 *             ,8?????$O          O?????$8   $$,   ,+?????????$~                                                         I???$?????    $$  ???????????$          ??????$=
 *             +??????8            =?????$      $$   ?????$??                                                               ???????  ?O    $????????$I            ???????
 *            ???????$            8????????,   ,$?  8????$$                                                                   $????$   8? 8?$$$???????$            8?????$
 *            $?????$            I$?????????$      ?$???$ ,                                                                     $?????~ =$?I   ,$??????$            $???$?$
 *           ???????            $?$?$  ??????$=  8????$                                    $8:                                    $????$? ?$  ?  ???????$            $?????$
 *          $?????$            $???8         8$$$???$:                                    :O88~                                    $$????, $=  8 $???$???$            $?????$
 *         $$????$            $????I   $I~    $$???8                 8                    O8888                          =           8???$  :  ????$  ????$            $?????8
 *        I??????            8??????$   8  $?8???$,                 OO                   ?888888                        ,8            ?$???8  ?$?+  8 $$???$           ,??????
 *        ?????$~           $???~=????    $$?????,                  + O                  8888888                       ?888~           +?$???I$~    ?$??????I           ??????$
 *       ???????           $???8  $??I?   ??????                                        =88888888                       888              ?$??$    ????????????           ??????8
 *      ~?????$           ,???$  $$  8?$  ?$??8                                         88888888O                                         ?????I??$    $??????            $?????
 *      ??????:           $??$   I  ?$???$????                                          888888888                                          $???$8       $?????$           ??????$
 *     $??????           $???$      ?$$?????8                                           888888888                                           $?  $  ??$ +,??????$           $?????+
 *     $?????           ???????$?      ???$8                                            888888888,                                           8? $?~ ,    ???????           ?$$???$
 *    ?$????$           $??$ 8$$???? ,+???$                                             888888888~                                            ?,       $????????$           $$????,
 *    ??????           ????      ~????????            ~O                                888888888?                                             $= , $???$  ??????$          ,$????$
 *   $??????$?         ????$$=     ,????$                                               888888888?         =O                                  ????$I      ???????         $$??????:
 *   ??????????$      $?$????$?$$~ ,$?$??                                               888888888?                                              $????  ,$?  ??????$     ~$??????????
 *   $????????????$   ??$?  ?$???8  ????                                                888888888?                                               $??$$??$:  ??????$  :$?????????????
 *  $???????????????$$???          ????=                                                888888888$                                               $??$I      $??????$$???????????????:
 *  $????????????????????$$$      +????                                  OO             888888888$                          =                     ???$   8$$??????????????????:?????$
 *  $????$ $???????????????????$??????8                                                 888888888$                        8888=                   $??????????????????????$$    ?????$
 * I??????    $$??????????????????????                                                  888888888$                         OO?                     ????????????????????$~      $?????
 * $?????       :$$?$????????????????$                                                  888888888$                                                 $?????????????????I         +??????
 * ??????           ?????????????????,                                                  8888,$888$                                                 $?????????????????           ?????$
 * ?????$          +????????????????$                                                 ? 8888 8888$$                                                 ?????????????????           ??????
 * ??????          $????????????????$                                                 8,8888 8888$8?                                                ?????????????????=          $????$
 * ?????8          $????????????????$                                                88:8888 8888$88:                                               $????????????????$          8????$
 * ?????$          $????????????????=                                               888:8888 8888$888                                               $?????????????????          $?????,
 * ?????I          ?$???????????????                                               ,888:8888 8888$8888                                              +????????????????$          ?????$+
 * ??????          ?????????????????                  :O8                          8888:8888 $888$88888                              ,,             =????????????????$          $????$I
 * ??????          ?????????????????                                              88888:8888 8888$88888                             O$:             ,$???????????????$          $??????
 * ??????          ?????????????????                                              88888:8888 8888$88888            8               $??              +????????????????$          $????$$
 * ??????          $$???????????????                                              88888 O888 $888 I8888          I8?                                =$????????????????          $????$+
 * ?????$          $????????????????~                                             888O ,? ~8,$+ :: $888                                             $?????????????????          $????$,
 * $????$          ?$??????????????$?                                             888, 888$$~88888: $88                                             $????????????????$          $?????
 * ?????$          $????????????????$                                             88?  88888?88888O ,88                                             ?????????????????+          $?????
 * ?????$          +$???????????????$                                             88   88888$88888+  ?8                                             ?????????????????           ??????
 * $?????           $????????????????                                             8    ~8888888888,   $                                            I????????????????$           ?????$
 * $?????           $????????????????8                                                 ,O8888888O$                                                 ?????????????????$          I??????
 * I??????$?$8      $?????????????????          ,$,$                                    8,O88$$==O                                                 ?????????????????$    +$?$????????
 *  ?????????$??$?$? ????????????????$8         888,               +?                  8I$88888 $O                                                ???????????????????$$?$???????????$
 *  ?$?????????????????????????????????           ,                                   8,?? 888:=?+?                       :O$                     $?????????????????????????????????$
 *  $??????????????????????????????????~                                              $$?$$$8O?8?$$                       O8OI                   $?????????????????????????????????$=
 *   ??????+$8$?????????????????????????                                              $????$$888 8$?                       ?                     ?????????????????????$??$$$  $?????
 *   ?????$,     ???$???????????????????8                                             $????$?8$$???I                                            8$?????????????????$$I        ??????
 *   $?????$           ??????????????????                                             $????$I ?????$                                           ~??????????????????           $?????=
 *    ??????           $?????????????????$                                            $????~I$?????$                                          ,$?????????????????$           ??????
 *    $$????8           $??????????????????                                           ??????????????                                          $?????????????????$           $?????=
 *     $?????           ?$?????????????????$                                         :?$????????????                                         $??????????????????:          =?????$
 *     $?????$           $??????????????????$                                        ?$?????????????:                                       ???????????????????$           $??????
 *      ??????            ????????????????????                                       ???????????????$                                      $???????????????????           ?????$$
 *      +?????$           =???????????????????8                                      ???????????????$                                     $??????????????????$            $????$
 *       ??????8           $$??????????????????$                                    $???????????????$~                                   $????????????????????           $?????$
 *        $$???$            ?????????????????????                                   $?????????????????                                  $???????????????????$           I?????$
 *        ?$???$?            $???????????????????$                                 +??????????????????                                ,????????????????????$           ,$?????~
 *         8?????$            $???????????????????$$                               $??????????????????$                              $?$??????????????????$            ??????$
 *          $?????$            $????????????????????$                             $?????????????????????                           ~?????????????????????$            $?????$
 *           ??????8            ?????????????????????$8                          ???????????????????????                          $$????????????????????$            $?????$
 *           ,$????$$            $???????????????8??????$                       +??????????????????????$$                       ?$?????????????????????$$$$$$$$$$???$??????
 *            ????????????????????????????????$$,  +$?????8                    8?????????????????????????$                    $?????????$  $??????????????????????????????=
 *             ?$?????????????????????????????8  ,?  ?$?????$                 $????????????????????????????$                $?????????? ,$  ? ???????????????????????????,
 *              8???????????????????????????$     ?  $$???????$$           ,$??????????????????????????????$??~         ????????????$~~+8 $? 8$??????????????????????????
 *               I$?????????????????????????$:+$$?, 8$??????????$?$$?$$$?$???$???????????????????????????????????????$??$?????????$?$ $??  :??$???$             $??????~
 *                8??????$             ~?????????$$  8 $$??????????????????????????????????????????????????????????????????????,8?+  ,??$$??????$              $$?????+
 *                 =???????              $???????? ,I : ?+$$???????????????????????????????????????????????????????????????????  8$$?  $???????$              $?????$
 *                  ,???????              :$?????$$ $?:OI ?$$????????????????????????????????????????????????????????$I?????$  $  ??:I8??????$              +???????:
 *                   ,?$?????$              8$??????$ $~ ~$::$??$+  $???????????????????????????????????????????????8   $??$ 8+I$   8??????8=              $??????$
 *                     ???????$               8$??????$ +???????  ? $????????????????????????????????????????????$??$ :   $$=:$  ?????????$              ,$??????8
 *                      ??????$??               8??????I$????$  ,?+ $?????????????????????????????????????????$  ???$  ?$  =$  ?$???????$               8??????$~
 *                        $????$??                ?$????????   ?,   $?   ???????????????????????????$?  $????$?  $??$    $~ :$$$?????$I                ???????$ ,
 *                         8???????$                ~$???????$$??$: $$  88$    8???$????????????????$    $?$  O$ ???$  $??$???????$$                 $???????8
 *                           ????????$                 $I?????????$ $ 8$ $= $$  $  I +$  8 ??   ?$??8 ??  $?  ??  $?? 8???????????$$$?             $???????$
 *                            ?????????I            =$??$I$$?????????$$:?$  ?8  ?     $  I??8$$ =???       ?$   ?=$???????????????????$$?        $????????+
 *                              $$$?????$?        $????$$?$$$8?$?????????$ $$ ??$ ,$$I$ :$?$ ?$ ~??$  $$?? ,$????????????$8$ $????????????$?   $???????$$
 *                                $$???????I   $???????????,    ~??$$????  $??????$$$$$+???$~ ? ,$?$?$????????????????$$,      ,$I????????????????????8
 *                                  $???????????????????8           :8$$??????????????????????????????????????????$$              ~$?????????????????
 *                                    8???????????????:                 $=?$$???????????????????????????????????$$                   ?$???????????$
 *                                     +$??????????$                   $??$?$?? ?8$??????????????????$88=  ?$??????                   8??????????
 *                                        $??????????I                ????????+           $$$????          ,8?????$$               $?????????$$
 *                                          $$?????????$$            8??????$+            $?????$            ?$?????$           $$??????$?$$?
 *                                             $??????????$$+       $??????$$             ??????$             $?????$$      I$???????????$
 *                                               =??$?$????????$=  ????????8              ???????,             $??????$ I$???????????$$
 *                                                  ?$$?$??????????????????               ???????+             ,??????????????????$?:
 *                                                      $$???????????????$?$,             ???????$          ,$$????????????????$$
 *                                                         :$????????????????$?????$$?$$I=$??????$$$$$?$???$???????????????$$,
 *                                                              $$?????????????????????????????????????????????????????$$
 *                                                                   ?$??????????????????????????????????????????$8=
 *                                                                          ?8$????????????????????????????8~
 *                                                                                    ,,+++??$+++,
 */
#endif
