// pinos.h

#pragma once

#ifndef _PINOS_H
#define _PINOS_H

#ifdef ARDUINO_AVR_MEGA2560
#define SD_CS_PIN 53
#else
#define SD_CS_PIN 10
#endif // ARDUINO_AVR_MEGA2560

#ifdef USING_SHIELD

#if USING_SHIELD == MEGA_STACK_DADOS_ACIONAMENTO_2019

#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/

#define HEAL_1 68	/*health 1*/
#define HEAL_2 62	/*health 2*/
#define HEAL_3 56	/*health 3*/
#define HEAL_4 58	/*health 4*/

#define buzzPin 54							//Pin that the buzzer is connected

#define PWMout 13							//Pin that the LED who shows apogee state is connected

// The LoRa Module in this board don't has M0, M1 or Aux

#elif USING_SHIELD == MEGA_STACK_DADOS_ACIONAMENTO_2020

#define IGN_1 36	/*act1*/
#define IGN_2 61	/*act2*/
#define IGN_3 46	/*act3*/
#define IGN_4 55	/*act4*/

#define HEAL_1 68	/*health 1*/
#define HEAL_2 62	/*health 2*/
#define HEAL_3 56	/*health 3*/
#define HEAL_4 58	/*health 4*/

#define buzzPin 22							//Pin that the buzzer is connected

#define PWMout 13							//Pin that the LED who shows apogee state is connected

#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
#define AUX_LORA_PIN 58 // Pinos adicionais do LoRa

#elif USING_SHIELD == MEGA_OCTA_PTH_MK_I

#define IGN_1 54	/*act1*/
#define IGN_2 56	/*act2*/
#define IGN_3 58	/*act3*/
#define IGN_4 60	/*act4*/

#define HEAL_1 55	/*health 1*/
#define HEAL_2 57	/*health 2*/
#define HEAL_3 59	/*health 3*/
#define HEAL_4 61	/*health 4*/

#define buzzPin 22							//Pin that the buzzer is connected

#define PWMout 13							//Pin that the LED who shows apogee state is connected

#define M0_LORA_PIN 12 // Pinos adicionais do LoRa
#define M1_LORA_PIN 11 // Pinos adicionais do LoRa
#define AUX_LORA_PIN 62 // Pinos adicionais do LoRa

#elif USING_SHIELD == ESP_ESSENTIALS_2025

#define M0_LORA_PIN D15 // Pinos adicionais do LoRa
#define M1_LORA_PIN D2 // Pinos adicionais do LoRa
#define AUX_LORA_PIN 62 // Pinos adicionais do LoRa

#define RX_LORA_ESP
#define TX_LORA_ESP

#define RX_GPS_ESP
#define TX_GPS_ESP

#elif USING_SHIELD == ESP_ESSENTIALS_2026

#define IGN_1 4	/*act1*/
#define IGN_2 6	/*act2*/
#define IGN_3 21	/*act3*/
#define IGN_4 48	/*act4*/

#define HEAL_1 5	/*health 1*/
#define HEAL_2 7	/*health 2*/
#define HEAL_3 2	/*health 3*/
#define HEAL_4 1	/*health 4*/

#define buzzPin 42							//Pin that the buzzer is connected

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

#undef SD_CS_PIN
#define SD_CS_PIN CS_SD_ESP

#elif USING_SHIELD == ESP_MAIN_SMD_2026

#define IGN_1 8	/*act1*/
#define IGN_2 17	/*act2*/
#define IGN_3 15	/*act3*/
#define IGN_4 6	/*act4*/

#define HEAL_1 18	/*health 1*/
#define HEAL_2 16	/*health 2*/
#define HEAL_3 7	/*health 3*/
#define HEAL_4 5	/*health 4*/

#define buzzPin 42							//Pin that the buzzer is connected

#define PWMout 35							//Pin that the LED who shows apogee state is connected

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

#undef SD_CS_PIN
#define SD_CS_PIN CS_SD_ESP

#else
#error: Placa selecionada não está disponível
#endif

#else
#error: É necessário selecionar uma placa com #define USING_SHIELD
#endif

#endif