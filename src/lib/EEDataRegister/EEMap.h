// EEMap.h  --  Mapa de enderecos da EEPROM

#pragma once

#ifndef _EE_MAP_h
#define _EE_MAP_h

#include "EEDataRegister.h"

// Slots de tamanho uniforme. E isto que permite ao mapa nao conhecer
// nenhum tipo de dado: quem guarda o que fica com cada dono, e aqui
// so existe "quantos slots" e "de que tamanho".
constexpr uint16_t EE_SLOT_SIZE  = 32;	// maior bloco em uso hoje tem 14 B
constexpr uint8_t  EE_SLOT_COUNT = 4;	// 2 em uso + folga
constexpr uint16_t EE_MAP_TOTAL  = uint16_t(EE_SLOT_SIZE) * EE_SLOT_COUNT;

// UM unico deslocamento por build para o mapa inteiro.
// Preserva o rodizio de enderecos a cada compilacao sem permitir que
// dois registros se sobreponham: todos deslocam juntos.
constexpr uint16_t EE_MAP_BASE = compileTimeLayoutBase(EE_MAP_TOTAL);

enum : uint8_t {
	EE_SLOT_APOGEU_ZERO = 0,
	EE_SLOT_LORA_CONFIG = 1
};

constexpr uint16_t eeSlotAddress(uint8_t slot) {
	return EE_MAP_BASE + uint16_t(slot) * EE_SLOT_SIZE;
}

static_assert(EE_MAP_BASE + EE_MAP_TOTAL <= _EEPROM_SIZE,
	"EEPROM: o mapa nao cabe na memoria disponivel");

#endif // _EE_MAP_h
