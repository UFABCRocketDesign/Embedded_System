// EEDataRegister.h

#pragma once

#ifndef _EE_DATA_REGISTER_h
#define _EE_DATA_REGISTER_h

#include <stdint.h>
#include <stddef.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#include <EEPROM.h>
#else
#include "WProgram.h"
#endif

// Definição do tamanho da EEPROM
#ifndef _EEPROM_SIZE
	#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ESP32) || defined(ESP8266)
		#define _EEPROM_SIZE 512
	#elif defined(E2END)
		#define _EEPROM_SIZE (static_cast<uint32_t>(E2END) + 1)
	#elif defined(EEPROM_SIZE)
		#define _EEPROM_SIZE EEPROM_SIZE
	#else
		#define _EEPROM_SIZE 1024
	#endif
#endif

// Funções constexpr para tempo e hash de compilação (C++11)
static constexpr uint8_t parseDateDayDigit(char c) {
    return (c >= '0' && c <= '9') ? (c - '0') : 0;
}

static constexpr uint32_t getCompileTimestampSeconds() {
    return (
        (10UL * (__TIME__[6] - '0') + (__TIME__[7] - '0')) +
        60UL * ((10UL * (__TIME__[3] - '0') + (__TIME__[4] - '0')) +
        60UL * ((10UL * (__TIME__[0] - '0') + (__TIME__[1] - '0')) +
        24UL * (10UL * parseDateDayDigit(__DATE__[4]) + parseDateDayDigit(__DATE__[5]))))
    );
}

static constexpr uint16_t hashStringC11(const char* str, uint16_t hash) {
    return (*str == '\0') ?
		hash : hashStringC11(str + 1, static_cast<uint16_t>((hash << 5) - hash + static_cast<uint8_t>(*str)));
}

static constexpr uint16_t compileTimeHashCalc() {
    return hashStringC11(__TIME__, hashStringC11(__DATE__, 0));
}

// Base pseudo-aleatoria para UM CONJUNTO de registros.
// Recebe o total de bytes do layout inteiro, de forma que todos os
// registros se desloquem JUNTOS, preservando o espacamento relativo.
static constexpr uint16_t compileTimeLayoutBase(uint16_t totalBytes) {
	return static_cast<uint16_t>(
		getCompileTimestampSeconds() %
		((_EEPROM_SIZE > totalBytes) ? (_EEPROM_SIZE - totalBytes + 1) : 1)
	);
}

template <typename type>
class EEDataRegister{
	static constexpr uint16_t expectedCompileHash = compileTimeHashCalc();

	// Metadados
	uint16_t eeAddress = 0;								// Endereço na EEPROM
	uint16_t totalSize = sizeof(EEDataRegister<type>);	// Tamanho (metadados + data)
	uint16_t compileHash = expectedCompileHash;			// Versão de compilação
	uint16_t checkSum = 0;								// Integridade dos dados

	// Permite que EEDataRegister<A> leia o endereco de EEDataRegister<B>
	template <typename U> friend class EEDataRegister;


	public:

	type data;

	EEDataRegister();
	explicit EEDataRegister(uint16_t address);

	static constexpr uint32_t getEEPROMSize() {
		return _EEPROM_SIZE;
	}

	static constexpr uint16_t blockSize() {
		return static_cast<uint16_t>(sizeof(EEDataRegister<type>));
	}

	static constexpr uint32_t addressAfter(uint16_t base) {
		return static_cast<uint32_t>(base) + blockSize();
	}

	static constexpr bool fitsAt(uint32_t base) {
		return (base + blockSize()) <= _EEPROM_SIZE;
	}


	// ATENCAO: considera SOMENTE este tipo, nao coordena com outros
	// registros. Para varios registros, use compileTimeLayoutBase()
	// com o total do conjunto, ou simplesmente base fixa.
	static constexpr uint16_t getCompileRefAddress() {
		return compileTimeLayoutBase(blockSize());
	}


	uint16_t computeDataCheckSum() const;
	void update();
	bool isValid() const;

	uint16_t getAddress() const { return eeAddress; }


	bool fitsInEEPROM() const;
	bool fitsInEEPROM(uint16_t address) const;

	bool load();
	bool load(uint16_t address);
	bool saveIfChanged();
	bool save();
	bool save(uint16_t address);


	bool placeAt(uint16_t address);

	template <typename PrevType>
	bool placeAfter(const EEDataRegister<PrevType>& prev);


	// 1. Checa se um registro de tipo NextType cabe a partir de um endereço base qualquer
	template <typename NextType>
	static bool canFitAfter(uint16_t baseAddress);

	// 2. Calcula o endereço imediatamente após o bloco atual e valida se cabe NextType
	template <typename NextType>
	bool getNextAvailableAddressFor(uint16_t &nextAddr) const;
};

#include "EEDataRegister.tpp"

#endif // _EE_DATA_REGISTER_h
