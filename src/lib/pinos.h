// pinos.h

#pragma once

#ifndef _PINOS_H
#define _PINOS_H

#ifdef USING_BOARD

	#if defined(ARDUINO_ARCH_AVR) // Check if compiller is set to an Arduino board

		#warning Utilizando placa em arquitetura AVR (como o Arduino)

		#if defined(ARDUINO_AVR_MEGA2560)

			#define SD_CS_PIN 53

			#if USING_BOARD == MEGA_STACK_DADOS_ACIONAMENTO_2019

				#define BOARD_HAS_LoRa_DORJI

				#define BOARD_HAS_IGN_1
				#define BOARD_HAS_IGN_2
				#define BOARD_HAS_IGN_3
				#define BOARD_HAS_IGN_4

				#define IGN_1 36	/*act1*/
				#define IGN_2 61	/*act2*/
				#define IGN_3 46	/*act3*/
				#define IGN_4 55	/*act4*/

				#define HEAL_1 68	/*health 1*/
				#define HEAL_2 62	/*health 2*/
				#define HEAL_3 56	/*health 3*/
				#define HEAL_4 58	/*health 4*/

				#define IGN_CMD HIGH	/* State which ign is on */

				#define buzzPin 54							//Pin that the buzzer is connected
				#define buzzCmd LOW							//State which buzzer is on

				#define PWMout 13							//Pin that the LED who shows apogee state is connected

				// The LoRa Module in this board don't has M0, M1 or Aux

			#elif USING_BOARD == MEGA_STACK_DADOS_ACIONAMENTO_2020

				#define BOARD_HAS_LoRa_E32

				#define BOARD_HAS_IGN_1
				#define BOARD_HAS_IGN_2
				#define BOARD_HAS_IGN_3
				#define BOARD_HAS_IGN_4

				#define IGN_1 36	/*act1*/
				#define IGN_2 61	/*act2*/
				#define IGN_3 46	/*act3*/
				#define IGN_4 55	/*act4*/

				#define HEAL_1 68	/*health 1*/
				#define HEAL_2 62	/*health 2*/
				#define HEAL_3 56	/*health 3*/
				#define HEAL_4 58	/*health 4*/

				#define IGN_CMD HIGH	/* State which ign is on */

				#define buzzPin 22							//Pin that the buzzer is connected
				#define buzzCmd LOW							//State which buzzer is on

				#define PWMout 13							//Pin that the LED who shows apogee state is connected

				#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
				#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
				#define AUX_LORA_PIN 58 // Pinos adicionais do LoRa

			#elif USING_BOARD == MEGA_OCTA_PTH_MK_I

				#define BOARD_HAS_LoRa_E32

				#define BOARD_HAS_IGN_1
				#define BOARD_HAS_IGN_2
				#define BOARD_HAS_IGN_3
				#define BOARD_HAS_IGN_4

				#define IGN_1 54	/*act1*/
				#define IGN_2 56	/*act2*/
				#define IGN_3 58	/*act3*/
				#define IGN_4 60	/*act4*/

				#define HEAL_1 55	/*health 1*/
				#define HEAL_2 57	/*health 2*/
				#define HEAL_3 59	/*health 3*/
				#define HEAL_4 61	/*health 4*/

				#define IGN_CMD HIGH	/* State which ign is on */

				#define buzzPin 22							//Pin that the buzzer is connected
				#define buzzCmd LOW							//State which buzzer is on

				#define PWMout 13							//Pin that the LED who shows apogee state is connected

				#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
				#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
				#define AUX_LORA_PIN 62 // Pinos adicionais do LoRa

			#else

				#error: A placa selecionada pode nao ser compativel com a compilacao definida (você trocou ESP com Arduino?)

			#endif

		#elif defined(ARDUINO_AVR_UNO)

			#define SD_CS_PIN 10

			#error: Nao existem shields definidas para Arduino Uno

		#elif defined(ARDUINO_AVR_NANO)

			#define SD_CS_PIN 10

			#error: Nao existem shields definidas para Arduino Nano

		#else

			#error: Compilador definido para placa Arduino desconhecia

		#endif

	#elif defined(ARDUINO_ARCH_ESP32) // Check if compiller is set to an ESP

		#warning Utilizando placa em arquitetura ESP32

		#define ESP32_WAIT_TIME_MS 50

		#if defined(ARDUINO_ESP32S3_DEV)

			#if USING_BOARD == ESP_ESSENTIALS_2025

				#define BOARD_HAS_LoRa_E32

				// #define IGN_1 	/*act1*/
				// #define IGN_2 	/*act2*/
				// #define IGN_3 	/*act3*/
				// #define IGN_4 	/*act4*/

				// #define HEAL_1 	/*health 1*/
				// #define HEAL_2 	/*health 2*/
				// #define HEAL_3 	/*health 3*/
				// #define HEAL_4 	/*health 4*/

				#define buzzPin 27							//Pin that the buzzer is connected
				#define buzzCmd LOW							//State which buzzer is on

				#define PWMout 12							//Pin that the LED who shows apogee state is connected

				#define M0_LORA_PIN 26 // Pinos adicionais do LoRa
				#define M1_LORA_PIN 25 // Pinos adicionais do LoRa
				#define AUX_LORA_PIN 36 // Pinos adicionais do LoRa

				#define RX_LORA_ESP 32
				#define TX_LORA_ESP 33

				#define RX_GPS_ESP 16
				#define TX_GPS_ESP 17

				#define MOSI_SD_ESP 23
				#define MISO_SD_ESP 19
				#define SCK_SD_ESP 18
				#define CS_SD_ESP 5

				#define SDA_I2C_ESP 21
				#define SCL_I2C_ESP 22

				#define SD_CS_PIN CS_SD_ESP


			#elif USING_BOARD == ESP_ESSENTIALS_2026

				#define BOARD_HAS_LoRa_E32

				#define BOARD_HAS_IGN_1
				#define BOARD_HAS_IGN_2
				#define BOARD_HAS_IGN_3
				#define BOARD_HAS_IGN_4

				#define IGN_1 4	/*act1*/
				#define IGN_2 6	/*act2*/
				#define IGN_3 21	/*act3*/
				#define IGN_4 47	/*act4*/

				#define HEAL_1 5	/*health 1*/
				#define HEAL_2 7	/*health 2*/
				#define HEAL_3 2	/*health 3*/
				#define HEAL_4 1	/*health 4*/

				#define IGN_CMD HIGH	/* State which ign is on */

				#define buzzPin 42							//Pin that the buzzer is connected
				#define buzzCmd LOW							//State which buzzer is on

				#define BOARD_HAS_RGB

				#define RGB_rPin 35
				#define RGB_gPin 36
				#define RGB_bPin 37

				#define RGB_CMD HIGH

				#define PWMout 35							//Pin that the LED who shows apogee state is connected

				#define M0_LORA_PIN 18 // Pinos adicionais do LoRa
				#define M1_LORA_PIN 17 // Pinos adicionais do LoRa
				#define AUX_LORA_PIN 41 // Pinos adicionais do LoRa

				#define RX_LORA_ESP 15
				#define TX_LORA_ESP 16

				#define RX_GPS_ESP 39
				#define TX_GPS_ESP 40

				#define MOSI_SD_ESP 11
				#define MISO_SD_ESP 13
				#define SCK_SD_ESP 12
				#define CS_SD_ESP 14

				#define SDA_I2C_ESP 8
				#define SCL_I2C_ESP 9

				#define SD_CS_PIN CS_SD_ESP

			#elif USING_BOARD == ESP_MAIN_SMD_2026

				#define BOARD_HAS_LoRa_E32

				#define BOARD_HAS_IGN_1
				#define BOARD_HAS_IGN_2
				#define BOARD_HAS_IGN_3
				#define BOARD_HAS_IGN_4

				#define IGN_1 8	/*act1*/
				#define IGN_2 17	/*act2*/
				#define IGN_3 15	/*act3*/
				#define IGN_4 6	/*act4*/

				#define HEAL_1 18	/*health 1*/
				#define HEAL_2 16	/*health 2*/
				#define HEAL_3 7	/*health 3*/
				#define HEAL_4 5	/*health 4*/

				#define IGN_CMD HIGH	/* State which ign is on */

				#define buzzPin 2 						//Pin that the buzzer is connected
				#define buzzCmd LOW							//State which buzzer is on

				#define PWMout 1							//Pin that the LED who shows apogee state is connected

				#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
				#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
				#define AUX_LORA_PIN 3 // Pinos adicionais do LoRa

				#define RX_LORA_ESP 9
				#define TX_LORA_ESP 10

				#define RX_GPS_ESP 21
				#define TX_GPS_ESP 47

				#define MOSI_SD_ESP 37
				#define MISO_SD_ESP 35
				#define SCK_SD_ESP 36
				#define CS_SD_ESP 38

				#define SDA_I2C_ESP 41
				#define SCL_I2C_ESP 42

				#define SD_CS_PIN CS_SD_ESP

			#elif USING_BOARD == ESP_JOHN_SI_SMD_2026

				#define BOARD_HAS_LoRa_E32

				#define BOARD_HAS_IGN_1
				#define BOARD_HAS_IGN_2
				#define BOARD_HAS_IGN_3
				#define BOARD_HAS_IGN_4

				#define IGN_1 6	/*act1*/
				#define IGN_2 4	/*act2*/
				#define IGN_3 36	/*act3*/
				#define IGN_4 35	/*act4*/

				#define HEAL_1 7	/*health 1*/
				#define HEAL_2 5	/*health 2*/
				#define HEAL_3 9	/*health 3*/
				#define HEAL_4 3	/*health 4*/

				#define IGN_CMD HIGH	/* State which ign is on */

				#define buzzPin 37							//Pin that the buzzer is connected
				#define buzzCmd HIGH							//State which buzzer is on

				#define PWMout 1							//Pin that the LED who shows apogee state is connected

				#define M0_LORA_PIN 16 // Pinos adicionais do LoRa
				#define M1_LORA_PIN 15 // Pinos adicionais do LoRa
				#define AUX_LORA_PIN 8 // Pinos adicionais do LoRa

				#define RX_LORA_ESP 18
				#define TX_LORA_ESP 17

				#define RX_GPS_ESP 21
				#define TX_GPS_ESP 47

				#define MOSI_SD_ESP 11
				#define MISO_SD_ESP 13
				#define SCK_SD_ESP 12
				#define CS_SD_ESP 14

				#define SDA_I2C_ESP 42
				#define SCL_I2C_ESP 41

				#define SD_CS_PIN CS_SD_ESP


			#else

				#error: A placa selecionada pode nao ser compativel com a compilacao definida (você trocou Arduino com ESP?)

			#endif

		#elif defined(ARDUINO_ESP32S2_DEV)

			#error: Nao existem shields definidas para ESP32 S2 (Nao confundir com versao S3, confira a placa selecionada)

		#else

			#error: Compilador definido para placa ESP desconhecia

		#endif

	#else

		#error Utilizando placa em arquitetura nao definida

	#endif

#else
	#error: Eh necessario selecionar uma placa com #define USING_BOARD
#endif

#endif