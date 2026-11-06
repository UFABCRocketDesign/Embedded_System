#include "LoRaConfig.h"

inline uint8_t LoRaConfig::asciiHex2Nibble(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return 0;
}

uint8_t LoRaConfig::nibble2AsciiHex(byte val){
	if (val < 10) return '0' + val;
	return 'A' + (val - 10);
}

uint8_t LoRaConfig::hexFromCharPair(const char* ptr) {
	return (asciiHex2Nibble(ptr[0]) << 4) | asciiHex2Nibble(ptr[1]);
}

void LoRaConfig::embedHexByte(char* target, byte val) {
	target[0] = nibble2AsciiHex(val >> 4);
	target[1] = nibble2AsciiHex(val & 0x0F);
}

Configuration LoRaConfig::buildConfig(
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
)
{
	Configuration cfg; // Criada vazia (assume os padrões da biblioteca)
	cfg.HEAD = 0x00;   // Geralmente 0xC0 ou 0xC2 para o E32
	cfg.ADDH = addh;
	cfg.ADDL = addl;

	cfg.SPED.airDataRate = airDataRate;
	cfg.SPED.uartBaudRate = uartBaudRate;
	cfg.SPED.uartParity = uartParity;

	cfg.CHAN = chan;

	cfg.OPTION.fec = fec;
	cfg.OPTION.fixedTransmission = fixedTransmission;
	cfg.OPTION.ioDriveMode = ioDriveMode;
	cfg.OPTION.transmissionPower = transmissionPower;
	cfg.OPTION.wirelessWakeupTime = wirelessWakeupTime;

	return cfg;
}


bool LoRaConfig::begin()
{
	bool ret = LoRaConfigurations.begin();

	loadDefaultConfigurations();
	EEValid = loadEEConfigurations();
	setConfigurations();

	return ret;
}



void LoRaConfig::updateFrequency(){
	HandshakeState oldState = hsState;

	int discardLimit = 256; // Safety limit to prevent infinite loops on serial noise

	switch (hsState) {
		case HS_IDLE: {
			if (internalLoRa.available() > 0) {
				while (internalLoRa.available() > 0 && internalLoRa.peek() != 'M' && discardLimit-- > 0) {
					internalLoRa.read();
				}
				if (internalLoRa.available() > 0 && internalLoRa.peek() == 'M') {
					startWait = millis();
					hsState = HS_WAITING_M_PACKET;
				}
			}
			break;
		}

		case HS_WAITING_M_PACKET: {
			if (internalLoRa.available() >= chgFreqReqLen) {
				char tempRec[64] = {};
				uint8_t count = internalLoRa.readBytesUntil('\n', tempRec, chgFreqReqLen);
				tempRec[count] = '\0';

				// Pause telemetry for 6s
				// extern unsigned long pauseTelemetryUntil;
				pauseTelemetryUntil = millis() + 6000;

	#if USE_PRINT
				Serial.println(F("\n========== [LORA RX] =========="));
				Serial.print(F("[LORA RX] Mensagem recebida: "));
				Serial.println(tempRec);
	#endif

				if (getConfigurations()) {
					previousConfig = currentConfig;
				} else {
					previousConfig = currentConfig;
				}

				bool reqCheck = true;
				reqCheck &= (strncmp(tempRec, chgFreqReqHead, chgFreqReqHeadLen) == 0);
				if (reqCheck) {
					reqCheck &= (strncmp(tempRec + chgFreqReqHeadLen + 2, chgFreqReqMid, chgFreqReqMidLen) == 0);
				}
				if (reqCheck) {
					reqCheck &= (strncmp(tempRec + chgFreqReqHeadLen + 2 + chgFreqReqMidLen + 4, chgFreqReqTail, chgFreqReqTailLen) == 0);
				}

				if (!reqCheck) {
	#if USE_PRINT
					Serial.println(F("[LORA RX] ERRO: Header/Separador/Tail invalido"));
	#endif
					cancelConfigurations(false, previousConfig);
					hsState = HS_IDLE;
					break;
				}

				CHAN = hexFromCharPair(tempRec + chgFreqReqHeadLen);
				if (CHAN > 0x45) {
	#if USE_PRINT
					Serial.println(F("[LORA RX] ERRO: Canal invalido"));
	#endif
					cancelConfigurations(false, previousConfig);
					hsState = HS_IDLE;
					break;
				}

				ADDH = hexFromCharPair(tempRec + chgFreqReqHeadLen + 2 + chgFreqReqMidLen);
				ADDL = hexFromCharPair(tempRec + chgFreqReqHeadLen + 2 + chgFreqReqMidLen + 2);

	#if USE_PRINT
				Serial.print(F("[LORA RX] Parse OK! CHAN="));
				Serial.print(CHAN, DEC);
				Serial.print(F(" ADDH=0x"));
				Serial.print(ADDH, HEX);
				Serial.print(F(" ADDL=0x"));
				Serial.println(ADDL, HEX);
	#endif

				// char toSend[chgFreqCfmLen + 1] = {};
				memset(toSend, 0, sizeof(toSend));
				memcpy(toSend, chgFreqCfmHead, chgFreqCfmHeadLen);
				embedHexByte(toSend + chgFreqCfmHeadLen, CHAN);
				memcpy(toSend + chgFreqCfmHeadLen + 2, chgFreqCfmMid, chgFreqCfmMidLen);
				embedHexByte(toSend + chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen, ADDH);
				embedHexByte(toSend + chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen + 2, ADDL);
				memcpy(toSend + chgFreqCfmHeadLen + 2 + chgFreqCfmMidLen + 4, chgFreqCfmTail, chgFreqCfmTailLen);
				toSend[chgFreqCfmLen] = '\0';

	#if USE_PRINT
				Serial.print(F("[LORA RX] Enviando confirmacao: "));
				Serial.println(toSend);
	#endif
				sendCfmAt = millis() + 900;
				hsState = HS_SENDING_CFM;

	// 				delay(900);
	// 				while (internalLoRa.available() > 0 && discardLimit-- > 0) internalLoRa.read();
	// 				internalLoRa.println(toSend);

	// #if USE_PRINT
	// 				Serial.println(F("[LORA RX] Aguardando 1SSO_MSM do GS..."));
	// #endif
	// 				stateTimeout = millis() + 5000;
	// 				hsState = HS_WAITING_1SSO;
			} else if (millis() - startWait >= 100) {
	#if USE_PRINT
				Serial.print(F("[LORA RX] Bytes insuficientes ou invalido: "));
				Serial.print(internalLoRa.available());
				Serial.print(F(" / "));
				Serial.print(chgFreqReqLen);
				Serial.println(F(". Descartando 'M'"));
	#endif
				internalLoRa.read();
				hsState = HS_IDLE;
			}
			break;
		}

		case HS_SENDING_CFM: {
			if(long (millis() - sendCfmAt) < 0) break; // Esperar mais

			while (internalLoRa.available() > 0 && discardLimit-- > 0) internalLoRa.read();
			internalLoRa.println(toSend);

	#if USE_PRINT
			Serial.println(F("[LORA RX] Aguardando 1SSO_MSM do GS..."));
	#endif
			stateTimeout = millis() + 5000;
			hsState = HS_WAITING_1SSO;
			break;
		}

		case HS_WAITING_1SSO: {
			if (millis() > stateTimeout) {
	#if USE_PRINT
				Serial.println(F("[LORA RX] TIMEOUT aguardando 1SSO_MSM"));
	#endif
				cancelConfigurations(false, previousConfig);
				hsState = HS_IDLE;
				break;
			}

			if (internalLoRa.available() > 0) {
				while (internalLoRa.available() > 0 && internalLoRa.peek() != '1' && discardLimit-- > 0) {
					internalLoRa.read();
				}
				if (internalLoRa.available() >= chgFreqOkLen) {
					uint8_t count = internalLoRa.readBytesUntil('\n', recieved, chgFreqOkLen);
					recieved[count] = '\0';
	#if USE_PRINT
					Serial.print(F("[LORA RX] Recebeu: "));
					Serial.println(recieved);
	#endif

					if (strncmp(recieved, chgFreqOk, chgFreqOkLen) == 0) {
						currentConfig.ADDL = ADDL;
						currentConfig.ADDH = ADDH;
						currentConfig.CHAN = CHAN;

						if (setConfigurations()) {
							saveEEConfigurations();
	#if USE_PRINT
							Serial.println(F("[LORA RX] Frequencia aplicada. Aguardando MUD0U_MSM do GS..."));
	#endif
							stateTimeout = millis() + 5000;
							hsState = HS_WAITING_MUD0U;
						} else {
							cancelConfigurations(true, previousConfig);
							hsState = HS_IDLE;
						}
					} else {
	#if USE_PRINT
						Serial.println(F("[LORA RX] ERRO: 1SSO_MSM invalido"));
	#endif
						cancelConfigurations(false, previousConfig);
						hsState = HS_IDLE;
					}
				}
			}
			break;
		}

		case HS_WAITING_MUD0U: {
			if (millis() > stateTimeout) {
	#if USE_PRINT
				Serial.println(F("[LORA RX] TIMEOUT aguardando MUD0U_MSM"));
	#endif
				cancelConfigurations(true, previousConfig);
				hsState = HS_IDLE;
				break;
			}

			if (internalLoRa.available() > 0) {
				while (internalLoRa.available() > 0 && internalLoRa.peek() != 'M' && discardLimit-- > 0) {
					internalLoRa.read();
				}
				if (internalLoRa.available() >= chgFreqVrfyLen) {
					uint8_t count = internalLoRa.readBytesUntil('\n', recieved, chgFreqVrfyLen);
					recieved[count] = '\0';
	#if USE_PRINT
					Serial.print(F("[LORA RX] Recebeu: "));
					Serial.println(recieved);
	#endif

					if (strncmp(recieved, chgFreqVrfy, chgFreqVrfyLen) == 0) {
	#if USE_PRINT
						Serial.println(F("[LORA RX] Enviando JUR0_JUR4D1NH0..."));
	#endif
						internalLoRa.println(TX_CHG_FREQ_RESP);
	#if USE_PRINT
						Serial.println(F("[LORA RX] Aguardando B04 do GS..."));
	#endif
						stateTimeout = millis() + 5000;
						hsState = HS_WAITING_B04;
					} else {
	#if USE_PRINT
						Serial.println(F("[LORA RX] ERRO: MUD0U_MSM invalido"));
	#endif
						cancelConfigurations(true, previousConfig);
						hsState = HS_IDLE;
					}
				}
			}
			break;
		}

		case HS_WAITING_B04: {
			if (millis() > stateTimeout) {
	#if USE_PRINT
				Serial.println(F("[LORA RX] TIMEOUT aguardando B04"));
	#endif
				cancelConfigurations(true, previousConfig);
				hsState = HS_IDLE;
				break;
			}

			if (internalLoRa.available() > 0) {
				while (internalLoRa.available() > 0 && internalLoRa.peek() != 'B' && discardLimit-- > 0) {
					internalLoRa.read();
				}
				if (internalLoRa.available() >= chgFreqFinalLen) {
					uint8_t count = internalLoRa.readBytesUntil('\n', recieved, chgFreqFinalLen);
					recieved[count] = '\0';
	#if USE_PRINT
					Serial.print(F("[LORA RX] Recebeu: "));
					Serial.println(recieved);
	#endif

					if (strncmp(recieved, chgFreqFinal, chgFreqFinalLen) == 0) {
	#if USE_PRINT
						Serial.println(F("[LORA RX] Mudanca de frequencia concluida com sucesso!"));
	#endif
					} else {
	#if USE_PRINT
						Serial.println(F("[LORA RX] ERRO: B04 invalido"));
	#endif
						cancelConfigurations(true, previousConfig);
					}
					hsState = HS_IDLE;
				}
			}
			break;
		}
	}

	if (hsState == HS_IDLE) {
		if (oldState != HS_IDLE) pauseTelemetryUntil = 0;
	} else {
		pauseTelemetryUntil = millis() + 6000;	// Renova enquanto o handshake estiver vivo
	}
}

bool LoRaConfig::setConfigurations()
{
	ResponseStructContainer c = LoRaConfigurations.getConfiguration();
#if USE_PRINT
	Serial.println(c.status.getResponseDescription());
	Serial.println(c.status.code);
#endif

	if(c.status.code != E32_SUCCESS) {
		c.close();
		return false;
	}

	// It's important get configuration pointer before all other operation
	Configuration configuration = *(Configuration*)c.data;

	//   printParameters(configuration);
	configuration.ADDL = currentConfig.ADDL;
	configuration.ADDH = currentConfig.ADDH;
	configuration.CHAN = currentConfig.CHAN;

	configuration.OPTION.fec = defaultConfig.OPTION.fec; // FEC_1_ON;
	configuration.OPTION.fixedTransmission = defaultConfig.OPTION.fixedTransmission; // FT_TRANSPARENT_TRANSMISSION;
	configuration.OPTION.ioDriveMode = defaultConfig.OPTION.ioDriveMode; // IO_D_MODE_PUSH_PULLS_PULL_UPS;
	configuration.OPTION.transmissionPower = defaultConfig.OPTION.transmissionPower; // POWER_20;
	configuration.OPTION.wirelessWakeupTime = defaultConfig.OPTION.wirelessWakeupTime; // WAKE_UP_250;

	configuration.SPED.airDataRate = defaultConfig.SPED.airDataRate; // AIR_DATA_RATE_101_192;
	// configuration.SPED.uartBaudRate = defaultConfig.SPED.uartBaudRate; // currentConfig.SPED.uartBaudRate;
	// configuration.SPED.uartParity = defaultConfig.SPED.uartParity; // currentConfig.SPED.uartParity;

	// Set configuration changed and set to not hold the configuration
	ResponseStatus rs = LoRaConfigurations.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
#if USE_PRINT
	Serial.println(rs.getResponseDescription());
	Serial.println(rs.code);
#endif
	//   printParameters(configuration);
	c.close();

	return rs.code == E32_SUCCESS;
}

bool LoRaConfig::getConfigurations()
{
	ResponseStructContainer c = LoRaConfigurations.getConfiguration();
	// It's important get configuration pointer before all other operation
	if(c.status.code != E32_SUCCESS)
	{
		c.close();
		return false;
	}

	currentConfig = *(Configuration*)c.data;

	c.close();

	return true;
}

void LoRaConfig::saveEEConfigurations(){
	loraReg.data = currentConfig;
	loraReg.saveIfChanged();
}

bool LoRaConfig::loadEEConfigurations() {
	if (!loraReg.load()) return false;
	if (!(loraReg.data.HEAD == 0xC0 || loraReg.data.HEAD == 0xC2)) return false;
	currentConfig = loraReg.data;
	return true;
}

void LoRaConfig::loadDefaultConfigurations()
{
	currentConfig.ADDL = defaultConfig.ADDL;
	currentConfig.ADDH = defaultConfig.ADDH;
	currentConfig.CHAN = defaultConfig.CHAN;

	// currentConfig.OPTION.fec = FEC_0_OFF;
	// currentConfig.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
	// currentConfig.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
	// currentConfig.OPTION.transmissionPower = POWER_17;
	// currentConfig.OPTION.wirelessWakeupTime = WAKE_UP_1250;

	// currentConfig.SPED.airDataRate = AIR_DATA_RATE_011_48;
	// currentConfig.SPED.uartBaudRate = UART_BPS_9600;
	// currentConfig.SPED.uartParity = MODE_00_8N1;
}

void LoRaConfig::cancelConfigurations(bool reverter, Configuration &previousConfig)
{
	internalLoRa.println(TX_CHG_FREQ_ERROR);
#if USE_PRINT
	Serial.println(F("[LORA RX] ERRO: Handshake abortado ou configuracao invalida!"));
#endif

	if(reverter){
		currentConfig = previousConfig;
		setConfigurations();
		saveEEConfigurations();
	}
}