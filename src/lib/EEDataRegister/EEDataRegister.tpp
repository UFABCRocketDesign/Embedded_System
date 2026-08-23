#pragma once

template <typename type> EEDataRegister<type>::EEDataRegister() :
    eeAddress(0),
	totalSize(sizeof(EEDataRegister<type>)),
	compileHash(expectedCompileHash),
	checkSum(0) {}

template <typename type> EEDataRegister<type>::EEDataRegister(uint16_t address) :
	eeAddress(address),
	totalSize(sizeof(EEDataRegister<type>)),
	compileHash(expectedCompileHash),
	checkSum(0) {}

template <typename type> uint16_t EEDataRegister<type>::computeDataCheckSum() const {
    uint16_t sum = 0;
    const auto* p = reinterpret_cast<const uint8_t*>(&data);
    for (size_t i = 0; i < sizeof(type); i++) {
        sum += p[i];
    }
    return sum;
}

template <typename type> void EEDataRegister<type>::update() {
    compileHash = expectedCompileHash;
    totalSize = sizeof(EEDataRegister<type>);
    checkSum = computeDataCheckSum();
}

template <typename type> bool EEDataRegister<type>::fitsInEEPROM() const {
    return fitsInEEPROM(this->eeAddress);
}

template <typename type> bool EEDataRegister<type>::fitsInEEPROM(uint16_t address) const {
    uint32_t endAddress = static_cast<uint32_t>(address) + sizeof(EEDataRegister<type>);
    return (endAddress <= getEEPROMSize());
}

template <typename type> bool EEDataRegister<type>::isValid() const {
    if (!fitsInEEPROM()) return false;
    if (compileHash != expectedCompileHash) return false;
    if (totalSize != sizeof(EEDataRegister<type>)) return false;
    if (checkSum != computeDataCheckSum()) return false;
    return true;
}

template <typename type> bool EEDataRegister<type>::load() {
    if (!fitsInEEPROM()) return false;
    EEPROM.get(eeAddress, *this);
    return isValid();
}

template <typename type> bool EEDataRegister<type>::load(uint16_t address) {
    this->eeAddress = address;
    return load();
}

template <typename type> bool EEDataRegister<type>::save() {
    if (!fitsInEEPROM()) return false;
    update();
    EEPROM.put(eeAddress, *this);

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ESP32) || defined(ESP8266)
    EEPROM.commit();
#endif

    return true;
}

template <typename type> bool EEDataRegister<type>::save(uint16_t address) {
    this->eeAddress = address;
    return save();
}

template <typename type> template <typename NextType>
bool EEDataRegister<type>::canFitAfter(uint16_t baseAddress) {
    uint32_t endAddress = static_cast<uint32_t>(baseAddress) + sizeof(EEDataRegister<NextType>);
    return (endAddress <= getEEPROMSize());
}

template <typename type> template <typename NextType>
bool EEDataRegister<type>::getNextAvailableAddressFor(uint16_t &nextAddr) const {
    uint32_t targetAddress = static_cast<uint32_t>(eeAddress) + totalSize;
    if (canFitAfter<NextType>(static_cast<uint16_t>(targetAddress))) {
        nextAddr = static_cast<uint16_t>(targetAddress);
        return true;
    }
    return false; // Não cabe na EEPROM física
}

template <typename type> template <typename NextType>
bool EEDataRegister<type>::getNextValidAddressFrom(uint16_t startAddress, uint16_t &validAddr) {
    // Se o startAddress atual já comporta o próximo registro, use-o
    if (canFitAfter<NextType>(startAddress)) {
        validAddr = startAddress;
        return true;
    }
    // Se estourou a memória, tenta dar wrap para o início da EEPROM (endereço 0)
    if (canFitAfter<NextType>(0)) {
        validAddr = 0;
        return true;
    }
    // O registro é maior do que a EEPROM inteira
    return false;
}