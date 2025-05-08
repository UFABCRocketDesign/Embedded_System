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

#else
#error: Placa selecionada não está disponível
#endif

#else
#error: É necessário selecionar uma placa com #define USING_SHIELD
#endif

#endif