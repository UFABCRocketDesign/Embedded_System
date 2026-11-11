// declarations.hpp

#pragma once

#ifndef _DECLARATIONS_HPP
#define _DECLARATIONS_HPP

#include "Helpful/Helpful.hpp" // Auxiliares de codigo

// #include "MediaMovel/MediaMovel.hpp" // Media Movel Legado [substituida por MovingAverage]
#include "MovingAverage/MovingAverage.hpp" // Media movel simples
#include "SerialFilter/SerialFilter.hpp" // Media movel em cascata

#if USE_VIRTUAL
#include "./VirtualBridge/VirtualBridge.hpp"
#endif // USE_VIRTUAL

#if USE_BARO
#if 1 < ((USE_VIRTUALBARO) + (USE_BMP085) + (USE_BMP280) + (USE_BMP388))
#error: Múltiplos barômetros definidos
#elif USE_VIRTUALBARO
#include "./VirtualBaro/VirtualBaro.hpp"	// Barometro Virtual
VirtualBaro baro;
#elif USE_BMP085
#include "./BMP085/BMP085.hpp"				// Barometro BMP085
BMP085 baro;									//Barometer object declaration
#elif USE_BMP280
#include "./BMP280/BMP280.hpp"				// Barometro BMP280
BMP280 baro;									//Barometer object declaration
#elif USE_BMP388
#include "./BMP388/BMP388.hpp"				// Barometro BMP388
BMP388 baro;									//Barometer object declaration
#endif // USE_BMP085 / USE_BMP280
// MovingAverage MM_baro[2]{ (2),(2) };			//Array declaration of the moving average filter objects
float MM_baro[2]{};
bool baroHasData = false;
#endif // USE_BARO

#if USE_APOGEE_DETECTION
#include "./Apogeu/Apogeu.hpp" // Processamento de altitude e deteccao de apogeu

Apogeu apg(10, 15, 50);						//Apogee checker object declaration
#define LapsMaxT 5							//Maximum time of delay until emergency state declaration by the delay in sensor response. (seconds)
#define EM_mainN_DELAY 60					// Seconds before forced deployment


#if USE_DEPLOY
#include "./MonoDeploy/MonoDeploy.hpp" // Acionamento de paraquedas simples

const bool MonoDeploy::command = IGN_ON_LEVEL;

#if USE_DUAL_DEPLOY
#define EM_drogN_DELAY 10					// Seconds before forced deployment
#endif // USE_DUAL_DEPLOY

#if USE_DROGUE_BACKUP || USE_MAIN_BACKUP
#define sysDelay 2.5
#endif // USE_DROGUE_BACKUP || USE_MAIN_BACKUP

#if USE_MAIN_BACKUP
#define EM_mainB_DELAY 65					// Seconds before forced deployment
#endif // USE_MAIN_BACKUP

#if USE_MAIN_DELAYED
#define sysDelay_main 1.0
#endif // USE_MAIN_DELAYED

#if USE_DROGUE_BACKUP
#define EM_drogB_DELAY 15					// Seconds before forced deployment
#endif // USE_DROGUE_BACKUP


#define pins_drogN (IGN_1, HEALTH_1) /*act1*/
#define pins_drogB (IGN_2, HEALTH_2) /*act2*/
#define pins_mainN (IGN_3, HEALTH_3) /*act3*/
#define pins_mainB (IGN_4, HEALTH_4) /*act4*/


struct Recovery
{
	static MonoDeploy mainN;

#if USE_DUAL_DEPLOY
	static MonoDeploy drogN;
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
	static MonoDeploy mainB;
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
	static MonoDeploy drogB;
#endif // USE_DROGUE_BACKUP

	static bool begin()
	{
		bool aux = true;
		aux &= mainN.begin();

#if USE_DUAL_DEPLOY
		aux &= drogN.begin();
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
		aux &= mainB.begin();
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
		aux &= drogB.begin();
#endif // USE_DROGUE_BACKUP

		return aux;
	}
	static void emergency(bool state)
	{
		if(state) MonoDeploy::sealApogee(true);
		mainN.emergency(state, EM_mainN_DELAY);

		#if USE_DUAL_DEPLOY
				drogN.emergency(state, EM_drogN_DELAY);
		#endif // USE_DUAL_DEPLOY

		#if USE_MAIN_BACKUP
			mainB.emergency(state, EM_mainB_DELAY);
		#endif // USE_MAIN_BACKUP

		#if USE_DROGUE_BACKUP
				drogB.emergency(state, EM_drogB_DELAY);
		#endif // USE_DROGUE_BACKUP

	}
	static bool getGlobalState()
	{
		bool aux = false;
		aux |= mainN.getGlobalState();

#if USE_DUAL_DEPLOY
		aux |= drogN.getGlobalState();
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
		aux |= mainB.getGlobalState();
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
		aux |= drogB.getGlobalState();
#endif // USE_DROGUE_BACKUP

		return aux;
	}
	static void refresh()
	{
		mainN.refresh();

#if USE_DUAL_DEPLOY
		drogN.refresh();
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
		mainB.refresh();
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
		drogB.refresh();
#endif // USE_DROGUE_BACKUP

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

#if USE_DUAL_DEPLOY
MonoDeploy Recovery::drogN pins_drogN;
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
MonoDeploy Recovery::mainB pins_mainB;
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
MonoDeploy Recovery::drogB pins_drogB;
#endif // USE_DROGUE_BACKUP

#else
	#warning Essa compilacao nao realiza disparo de paraquedas
#endif // USE_DEPLOY

#endif // USE_APOGEE_DETECTION


#if USE_ACCEL
#if 1 < ((USE_VIRTUALACCEL) + (USE_ADXL345) + (USE_MPU9250_ACCEL) + (USE_ICM20948_ACCEL))
#error: Múltiplos acelerômetros definidos
#elif USE_VIRTUALACCEL
#include "./VirtualAccel/VirtualAccel.hpp"	// Acelerometro Virtual
VirtualAccel accel;
#elif USE_ADXL345
#include "./ADXL345/ADXL345.hpp" // Accelerometer ADXL345
ADXL345 accel;									//Accelerometer object declaration
#elif USE_MPU9250_ACCEL
#include "./MPU9250_ACCEL/MPU9250_ACCEL.hpp" // Accelerometer MPU9250_ACCEL
MPU9250_ACCEL accel(16);									//Accelerometer object declaration
#elif USE_ICM20948_ACCEL
#include "./ICM20948_ACCEL/ICM20948_ACCEL.hpp" // Accelerometer MPU9250_ACCEL
ICM20948_ACCEL accel(16);									//Accelerometer object declaration
#endif // USE_ADXL345 / USE_MPU9250_ACCEL
//MovingAverage MM_accel[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_accel[3]{};
#endif // USE_ACCEL

#if USE_GYRO
#if 1 < ((USE_VIRTUALGYRO) + (USE_L3G4200D) + (USE_MPU9250_GYRO) + (USE_ICM20948_GYRO))
#error: Múltiplos giroscópios definidos
#elif USE_VIRTUALGYRO
#include "./VirtualGyro/VirtualGyro.hpp"	// Gyroscope Virtual
VirtualGyro giro;
#elif USE_L3G4200D
#include "./L3G4200D/L3G4200D.hpp" // Gyroscope L3G4200D
L3G4200D giro(2000);							//Gyroscope object declaration
#elif USE_MPU9250_GYRO
#include "./MPU9250_GYRO/MPU9250_GYRO.hpp" // Gyroscope MPU9250_GYRO
MPU9250_GYRO giro(2000);							//Gyroscope object declaration
#elif USE_ICM20948_GYRO
#include "./ICM20948_GYRO/ICM20948_GYRO.hpp" // Gyroscope MPU9250_GYRO
ICM20948_GYRO giro(2000);							//Gyroscope object declaration
#endif // USE_L3G4200D / USE_MPU9250_GYRO
//MovingAverage MM_giro[3]{ (5),(5),(5) };	//Array declaration of the moving average filter objects
float MM_giro[3]{};
#endif // USE_GYRO

#if USE_MAGN
#if 1 < ((USE_VIRTUALMAGN) + (USE_HMC5883) + (USE_AK8963) + (USE_AK09916))
#error: Múltiplos magnetômetros definidos
#elif USE_VIRTUALMAGN
#include "./VirtualMagn/VirtualMagn.hpp"	// Magnetometro Virtual
VirtualMagn magn;
#elif USE_HMC5883
#include "./HMC5883/HMC5883.hpp" // Magnetometer HMC5883
HMC5883 magn;									//Magnetometer object declaration
#elif USE_AK8963
#include "./AK8963/AK8963.hpp" // Magnetometer AK8963
AK8963 magn;									//Magnetometer object declaration
#elif USE_AK09916
#include "./AK09916/AK09916.hpp" // Magnetometer USE_AK09916
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
#include "./SDCH/SDCH.hpp" // Auxiliar para gerenciamento de cartao SD

#if ARDUINO_ARCH_ESP32
SDCH SDC(SD_CS_PIN, CURRENT_MODE_PROJECT_NAME, "txt", SPI_SD);						//Declaration of object to help SD card file management
#else
SDCH SDC(SD_CS_PIN, CURRENT_MODE_PROJECT_NAME);						//Declaration of object to help SD card file management
#endif // ARDUINO_ARCH_ESP32
#endif // USE_STORAGE


#if USE_GNSS
#if 1 < ((USE_VIRTUALGNSS) + (USE_GNSS_NEO_M))
#error: Multiplos GPS definidos
#elif USE_VIRTUALGNSS
#include "./VirtualGPS/VirtualGPS.hpp"	// GPS Virtual
VirtualGPS GpS;
#elif USE_GNSS_NEO_M
#include "./GyGPS/GyGPS.hpp"			// Auxiliar para GPS
#ifdef ARDUINO_ARCH_ESP32
HardwareSerial GpSSerial(1);
GyGPS GpS(GpSSerial, 0, SERIAL_8N1, RX_GPS_ESP, TX_GPS_ESP);
#else
GyGPS GpS(Serial1, 0);
#endif // ARDUINO_ARCH_ESP32
#endif // USE_VIRTUALGNSS / USE_GNSS_NEO_M
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

#include "./LoRaConfig/LoRaConfig.hpp"

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

#if USE_BOARD_LINK
#include "ComProtocol/ComProtocol.hpp" // Protocolo de comunicacao entre placas [descontinuado]
ComProtocol Talk(Serial2, 9600);			//Declaration of communication protocol object
#endif // USE_BOARD_LINK

Helpful Gutil;								//Declaration of helpful object to general cases

#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT
#define RBF_PIN 2							//Pin that the USE_SOFTWARE_REMOVE_BEFORE_FLIGHT system is connected
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT

#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
unsigned short sysC = 0;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT


#if USE_BUZZER
#if 1 < ((USE_BUZZER_ACTIVE) + (USE_BUZZER_PASSIVE) + (USE_BUZZER_AS_LED))
#error: Multiplos tipos de buzzer definidos
#endif

#if USE_BUZZER_AS_LED
#define BUZZER_PIN LED_BUILTIN
#define BUZZER_ON_LEVEL HIGH
#endif // USE_BUZZER_AS_LED
#endif // USE_BUZZER

#if USE_MORSE_MSG

// #define MORSE_INTERRUPT (1)				// Use interrupt to avoid beep bugs

#include "./Morse/Morse.hpp"

#define ALARM_DELAY 10					// Delay after alarm when all systems working properly

#if USE_BUZZER_ACTIVE
MorseAtvBzz mensageiro(BUZZER_PIN, BUZZER_ON_LEVEL, "~ ");
// MorseAtvBzz mensageiro(BUZZER_PIN, BUZZER_ON_LEVEL, "a 1 - . ~ . ^ . < . # . > . [ . ] . { . }");
#elif USE_BUZZER_PASSIVE
Morse mensageiro(BUZZER_PIN, "~ ");
// Morse mensageiro(BUZZER_PIN, CURRENT_MODE_PROJECT_NAME);
#endif  // USE_BUZZER_ACTIVE / USE_BUZZER_PASSIVE

#if MORSE_INTERRUPT
MORSE_INTERRPUT_PRESET(mensageiro);
#endif // MORSE_INTERRUPT

Helpful Mutil;
#endif // USE_MORSE_MSG

#if USE_BEEP_SYSTEM_COUNT
#define holdT .1
Helpful beeper;
#endif // USE_BEEP_SYSTEM_COUNT

#if HAS_COMM_CHANNEL
template <typename T> void transmit(T message);
template <typename T> void transmitln(T message);
template <typename T, typename R> void transmit(T message, R value);
template <typename T, typename R> void transmitln(T message, R value);
#endif // HAS_COMM_CHANNEL

#if (LOG_RESET_REASON || REPORT_RESET_REASON) && defined(ARDUINO_ARCH_ESP32)
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
#endif // (LOG_RESET_REASON || REPORT_RESET_REASON) && defined(ARDUINO_ARCH_ESP32)



#endif // _DECLARATIONS_HPP