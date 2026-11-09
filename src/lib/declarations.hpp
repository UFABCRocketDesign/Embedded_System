// declarations.hpp

#pragma once

#ifndef _DECLARATIONS_HPP
#define _DECLARATIONS_HPP

// #include "./Classes.h"

#include "Helpful/Helpful.h" // Auxiliares de codigo


// #include "MediaMovel/MediaMovel.h" // Media Movel Legado [substituida por MovingAverage]
#include "MovingAverage/MovingAverage.h" // Media movel simples
#include "SerialFilter/SerialFilter.h" // Media movel em cascata

#if USE_VIRTUAL
#include "./VirtualBridge/VirtualBridge.h"
#endif // USE_VIRTUAL

#if USE_BARO
#if 1 < ((USE_V_BARO) + (USE_BMP085) + (USE_BMP280) + (USE_BMP388))
#error: Múltiplos barômetros definidos
#elif USE_V_BARO
#include "./VirtualBaro/VirtualBaro.h"	// Barometro Virtual
VirtualBaro baro;
#elif USE_BMP085
#include "./BMP085/BMP085.h"				// Barometro BMP085
BMP085 baro;									//Barometer object declaration
#elif USE_BMP280
#include "./BMP280/BMP280.h"				// Barometro BMP280
BMP280 baro;									//Barometer object declaration
#elif USE_BMP388
#include "./BMP388/BMP388.h"				// Barometro BMP388
BMP388 baro;									//Barometer object declaration
#endif // USE_BMP085 / USE_BMP280
// MovingAverage MM_baro[2]{ (2),(2) };			//Array declaration of the moving average filter objects
float MM_baro[2]{};
bool baroHasData = false;
#endif // USE_BARO

#if ApoGee
#include "./Apogeu/Apogeu.h" // Processamento de altitude e deteccao de apogeu

Apogeu apg(10, 15, 50);						//Apogee checker object declaration
#define LapsMaxT 5							//Maximum time of delay until emergency state declaration by the delay in sensor response. (seconds)
#define EM_mainN_DELAY 60					// Seconds before forced deployment


#if AnyDeploy
#include "./MonoDeploy/MonoDeploy.h" // Acionamento de paraquedas simples

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


#if USE_ACCEL
#if 1 < ((USE_V_ACCEL) + (USE_ADXL345) + (USE_MPU9250_ACCEL) + (USE_ICM20948_ACCEL))
#error: Múltiplos acelerômetros definidos
#elif USE_V_ACCEL
#include "./VirtualAccel/VirtualAccel.h"	// Acelerometro Virtual
VirtualAccel accel;
#elif USE_ADXL345
#include "./ADXL345/ADXL345.h" // Accelerometer ADXL345
ADXL345 accel;									//Accelerometer object declaration
#elif USE_MPU9250_ACCEL
#include "./MPU9250_ACCEL/MPU9250_ACCEL.h" // Accelerometer MPU9250_ACCEL
MPU9250_ACCEL accel(16);									//Accelerometer object declaration
#elif USE_ICM20948_ACCEL
#include "./ICM20948_ACCEL/ICM20948_ACCEL.h" // Accelerometer MPU9250_ACCEL
ICM20948_ACCEL accel(16);									//Accelerometer object declaration
#endif // USE_ADXL345 / USE_MPU9250_ACCEL
//MovingAverage MM_accel[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_accel[3]{};
#endif // USE_ACCEL

#if USE_GYRO
#if 1 < ((USE_V_GYRO) + (USE_L3G4200D) + (USE_MPU9250_GYRO) + (USE_ICM20948_GYRO))
#error: Múltiplos giroscópios definidos
#elif USE_V_GYRO
#include "./VirtualGyro/VirtualGyro.h"	// Gyroscope Virtual
VirtualGyro giro;
#elif USE_L3G4200D
#include "./L3G4200D/L3G4200D.h" // Gyroscope L3G4200D
L3G4200D giro(2000);							//Gyroscope object declaration
#elif USE_MPU9250_GYRO
#include "./MPU9250_GYRO/MPU9250_GYRO.h" // Gyroscope MPU9250_GYRO
MPU9250_GYRO giro(2000);							//Gyroscope object declaration
#elif USE_ICM20948_GYRO
#include "./ICM20948_GYRO/ICM20948_GYRO.h" // Gyroscope MPU9250_GYRO
ICM20948_GYRO giro(2000);							//Gyroscope object declaration
#endif // USE_L3G4200D / USE_MPU9250_GYRO
//MovingAverage MM_giro[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_giro[3]{};
#endif // USE_GYRO

#if USE_MAGN
#if 1 < ((USE_V_MAGN) + (USE_HMC5883) + (USE_AK8963) + (USE_AK09916))
#error: Múltiplos magnetômetros definidos
#elif USE_V_MAGN
#include "./VirtualMagn/VirtualMagn.h"	// Magnetometro Virtual
VirtualMagn magn;
#elif USE_HMC5883
#include "./HMC5883/HMC5883.h" // Magnetometer HMC5883
HMC5883 magn;									//Magnetometer object declaration
#elif USE_AK8963
#include "./AK8963/AK8963.h" // Magnetometer AK8963
AK8963 magn;									//Magnetometer object declaration
#elif USE_AK09916
#include "./AK09916/AK09916.h" // Magnetometer USE_AK09916
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
#include "./SDCH/SDCH.h" // Auxiliar para gerenciamento de cartao SD

#if ARDUINO_ARCH_ESP32
SDCH SDC(SD_CS_PIN, CURRENT_MODE_PROJECT_NAME, "txt", SPI_SD);						//Declaration of object to help SD card file management
#else
SDCH SDC(SD_CS_PIN, CURRENT_MODE_PROJECT_NAME);						//Declaration of object to help SD card file management
#endif // ARDUINO_ARCH_ESP32
#endif // USE_STORAGE


#if USE_GNSS
#if 1 < ((USE_V_GNSS) + (USE_NEO_M))
#error: Multiplos GPS definidos
#elif USE_V_GNSS
#include "./VirtualGPS/VirtualGPS.h"	// GPS Virtual
VirtualGPS GpS;
#elif USE_NEO_M
#include "./GyGPS/GyGPS.h"			// Auxiliar para GPS
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

#include "./LoRaConfig/LoRaConfig.h"

#define FREQUENCY_900
#define LoRa_ADDL 0x2A // 42 decimal (Standardized)
#define LoRa_ADDH 0x00
#define LoRa_CHAN 0x2A // 42 decimal (904 - 862)

LoRaConfig configLoRa(LoRa, byte(AUX_LORA_PIN), byte(M0_LORA_PIN), byte(M1_LORA_PIN), UART_BPS_RATE(LoRaBaudRate), byte(LoRa_ADDL), byte(LoRa_ADDH), byte(LoRa_CHAN));

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
#include "ComProtocol/ComProtocol.h" // Protocolo de comunicacao entre placas [descontinuado]
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

#include "./Morse/Morse.h"

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



#endif // _DECLARATIONS_HPP