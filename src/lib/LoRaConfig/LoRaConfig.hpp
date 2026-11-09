// LoRaConfig.h

#pragma once

#ifndef _LORA_CONFIG_h
#define _LORA_CONFIG_h

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

#include "LoRa_E32.h"
#include "../EEDataRegister/EEDataRegister.hpp"
#include "../EEDataRegister/EEMap.hpp"

class LoRaConfig
{

	constexpr static const char* chgFreqReqHead = RX_CHG_FREQ_REQ_HEAD;
	constexpr static size_t chgFreqReqHeadLen = sizeof(RX_CHG_FREQ_REQ_HEAD) - 1;

	constexpr static const char* chgFreqReqMid = RX_CHG_FREQ_REQ_MID;
	constexpr static size_t chgFreqReqMidLen = sizeof(RX_CHG_FREQ_REQ_MID) - 1;

	constexpr static const char* chgFreqReqTail = RX_CHG_FREQ_REQ_TAIL;
	constexpr static size_t chgFreqReqTailLen = sizeof(RX_CHG_FREQ_REQ_TAIL) - 1;

	constexpr static size_t chgFreqReqLen = chgFreqReqHeadLen + 2 + chgFreqReqMidLen + 4 + chgFreqReqTailLen;

	constexpr static const char* chgFreqCfmHead = TX_CHG_FREQ_CONFIRM_HEAD;
	constexpr static size_t chgFreqCfmHeadLen = sizeof(TX_CHG_FREQ_CONFIRM_HEAD) - 1;

	constexpr static const char* chgFreqCfmMid = TX_CHG_FREQ_CONFIRM_MID;
	constexpr static size_t chgFreqCfmMidLen = sizeof(TX_CHG_FREQ_CONFIRM_MID) - 1;

	constexpr static const char* chgFreqCfmTail = TX_CHG_FREQ_CONFIRM_TAIL;
	constexpr static size_t chgFreqCfmTailLen = sizeof(TX_CHG_FREQ_CONFIRM_TAIL) - 1;

	constexpr static size_t chgFreqCfmLen = chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen + 4 + chgFreqCfmTailLen;

	constexpr static const char* chgFreqOk = RX_CHG_FREQ_OK;
	constexpr static size_t chgFreqOkLen = sizeof(RX_CHG_FREQ_OK) - 1;

	constexpr static const char* chgFreqVrfy = RX_CHG_FREQ_VRFY;
	constexpr static size_t chgFreqVrfyLen = sizeof(RX_CHG_FREQ_VRFY) - 1;

	constexpr static const char* chgFreqFinal = RX_CHG_FREQ_FINAL;
	constexpr static size_t chgFreqFinalLen = sizeof(RX_CHG_FREQ_FINAL) - 1;

	enum HandshakeState {
		HS_IDLE,
		HS_WAITING_M_PACKET,
		HS_SENDING_CFM,
		HS_WAITING_1SSO,
		HS_WAITING_MUD0U,
		HS_WAITING_B04
	};

	HardwareSerial &internalLoRa;

	LoRa_E32 LoRaConfigurations;

	const Configuration defaultConfig;
	Configuration currentConfig;
	Configuration previousConfig;

	EEDataRegister<Configuration> loraReg;

	static_assert(EEDataRegister<Configuration>::blockSize() <= EE_SLOT_SIZE,
	"LoRa: bloco de configuracao excede o slot");

	unsigned long pauseTelemetryUntil = 0;

	HandshakeState hsState = HS_IDLE;

	unsigned long stateTimeout = 0;
	unsigned long startWait = 0;
	int CHAN = defaultConfig.CHAN;
	int ADDH = defaultConfig.ADDH;
	int ADDL = defaultConfig.ADDL;
	char recieved[64] = {};
	char toSend[chgFreqCfmLen + 1] = {};
	unsigned long sendCfmAt = 0;
	bool EEValid = false;


	inline uint8_t asciiHex2Nibble(char c);
	uint8_t nibble2AsciiHex(byte val);
	uint8_t hexFromCharPair(const char* ptr);
	void embedHexByte(char* target, byte val);

	static Configuration buildConfig(
		byte addh,
		byte addl,
		AIR_DATA_RATE airDataRate,
		UART_BPS_TYPE uartBaudRate,
		E32_UART_PARITY uartParity,
		byte chan,
		FORWARD_ERROR_CORRECTION_SWITCH fec,
		FIDEX_TRANSMISSION fixedTransmission,
		IO_DRIVE_MODE ioDriveMode,
		TRANSMISSION_POWER transmissionPower,
		WIRELESS_WAKE_UP_TIME wirelessWakeupTime
	);


	public:
	LoRaConfig(HardwareSerial &LoRa, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE baudRate, Configuration config):
		internalLoRa(LoRa),
		LoRaConfigurations(&LoRa, auxPin, m0Pin, m0Pin, baudRate),
		loraReg(eeSlotAddress(EE_SLOT_LORA_CONFIG)),
		defaultConfig(config)
	{}

	LoRaConfig(HardwareSerial &LoRa, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE baudRate,
		byte addl,
		byte addh,
		byte chan,
		FORWARD_ERROR_CORRECTION_SWITCH fec = FEC_1_ON,
		FIDEX_TRANSMISSION fixedTransmission = FT_TRANSPARENT_TRANSMISSION,
		IO_DRIVE_MODE ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS,
		TRANSMISSION_POWER transmissionPower = POWER_20,
		WIRELESS_WAKE_UP_TIME wirelessWakeupTime = WAKE_UP_250,
		AIR_DATA_RATE airDataRate = AIR_DATA_RATE_101_192,
		UART_BPS_TYPE uartBaudRate = UART_BPS_9600,
		E32_UART_PARITY uartParity = MODE_00_8N1
	) : LoRaConfig(LoRa, auxPin, m0Pin, m1Pin, baudRate, buildConfig(
		addh,
		addl,
		airDataRate,
		uartBaudRate,
		uartParity,
		chan,
		fec,
		fixedTransmission,
		ioDriveMode,
		transmissionPower,
		wirelessWakeupTime
	))
	{}

	bool begin();


	inline bool getEEValid() { return EEValid; }
	inline uint16_t getEEAddress() { return loraReg.getAddress(); }

	inline void discardPauseTelemetry() { pauseTelemetryUntil = 0; } // Garante telemetria ativa ao entrar em voo
	inline bool isTelemetryPaused() { return millis() < pauseTelemetryUntil; }

	const Configuration& getCurrentConfigurations() const { return currentConfig; }

	void updateFrequency();

	protected:

	bool setConfigurations();
	bool getConfigurations();

	void saveEEConfigurations();
	bool loadEEConfigurations();

	void loadDefaultConfigurations();

	void cancelConfigurations(bool reverter, Configuration &previousConfig);

};

#endif // _LORA_CONFIG_h

#if 0

struct Speed {
  uint8_t airDataRate		: 3; //bit 0-2
  uint8_t uartBaudRate		: 3; //bit 3-5
  uint8_t uartParity		: 2; //bit 6-7
};

struct Option {
	byte transmissionPower	: 2; //bit 0-1
	byte fec				: 1; //bit 2
	byte wirelessWakeupTime : 3; //bit 3-5
	byte ioDriveMode		: 1; //bit 6
	byte fixedTransmission	: 1; //bit 7
};

struct Configuration {
	byte HEAD = 0;
	byte ADDH = 0;
	byte ADDL = 0;
	struct Speed SPED; // Byte
	byte CHAN = 0;
	struct Option OPTION; // Byte
};

#endif