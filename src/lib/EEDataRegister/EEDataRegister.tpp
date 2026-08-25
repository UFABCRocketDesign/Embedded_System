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
	const uint16_t requested = eeAddress;

	if (!fitsInEEPROM(requested)) return false;

	EEPROM.get(requested, *this);

	const bool addressMatches = (eeAddress == requested);
	eeAddress = requested;					// nunca herdar lixo da EEPROM

	return addressMatches && isValid();
}


template <typename type> bool EEDataRegister<type>::load(uint16_t address)
{
	if (!fitsInEEPROM(address)) return false;
	this->eeAddress = address;
	return load();
}

template <typename type> bool EEDataRegister<type>::saveIfChanged()
{
    EEDataRegister<type> stored;
    if (stored.load(eeAddress) && memcmp(&stored.data, &data, sizeof(type)) == 0)
        return true;                     // ja identico: nao gasta escrita
    return save();
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
	if (!fitsInEEPROM(address)) return false;
	this->eeAddress = address;
	return save();
}

template <typename type> template <typename NextType>
bool EEDataRegister<type>::canFitAfter(uint16_t baseAddress) {
	return EEDataRegister<NextType>::fitsAt(static_cast<uint32_t>(baseAddress));
}

template <typename type> template <typename NextType>
bool EEDataRegister<type>::getNextAvailableAddressFor(uint16_t &nextAddr) const {
	const uint32_t targetAddress = static_cast<uint32_t>(eeAddress) + totalSize;
	if (!EEDataRegister<NextType>::fitsAt(targetAddress)) return false;
	nextAddr = static_cast<uint16_t>(targetAddress);
	return true;
}


template <typename type> bool EEDataRegister<type>::placeAt(uint16_t address) {
	if (!fitsInEEPROM(address)) return false;
	eeAddress = address;
	return true;
}

template <typename type> template <typename PrevType>
bool EEDataRegister<type>::placeAfter(const EEDataRegister<PrevType>& prev) {
	const uint32_t target = static_cast<uint32_t>(prev.eeAddress) + sizeof(EEDataRegister<PrevType>);
	if (!fitsAt(target)) return false;
	eeAddress = static_cast<uint16_t>(target);
	return true;
}
