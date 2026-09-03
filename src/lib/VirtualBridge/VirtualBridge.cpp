#include "VirtualBridge.h"

// A instancia do sistema. A porta vem de VB_SERIAL_PORT (default: Serial).
VirtualBridge& Bridge = VirtualBridge::getInstance(VB_SERIAL_PORT);

VirtualBridge::VirtualBridge(Stream &serialPort)
	: stream(serialPort), sensorCount(0), bufferIdx(0), discarding(false)
#if VB_DIAGNOSTICS
	, rejectedCount(0), lastRejectReason(REJ_NONE)
#endif
{
	for (uint8_t i = 0; i < VB_MAX_SENSORS; i++) {
		sensors[i].key[0] = '\0';
		sensors[i].value   = 0.0f;
		sensors[i].seq     = 0;
		sensors[i].updated = false;
	}
}

VirtualBridge& VirtualBridge::getInstance(Stream &serialPort)
{
	static VirtualBridge instance(serialPort);
	return instance;
}


#if VB_DIAGNOSTICS
void VirtualBridge::reject(RejectReason r)
{
	lastRejectReason = r;
	rejectedCount++;
}
#endif // VB_DIAGNOSTICS

void VirtualBridge::update()
{
	while (stream.available() > 0) {
		char c = (char)stream.read();

		if (c == '\n' || c == '\r') {
			if (discarding) {			// fim da linha estourada: agora sim volta ao normal
				discarding = false;
				bufferIdx  = 0;
				reject(REJ_OVERFLOW);
				continue;
			}
			if (bufferIdx > 0) {
				rxBuffer[bufferIdx] = '\0';
				parsePacket(rxBuffer);
				bufferIdx = 0;
			}
			continue;
		}

		if (discarding) continue;		// linha perdida: ignora ate o proximo delimitador

		if (bufferIdx < VB_BUFFER_LEN - 1) {
			rxBuffer[bufferIdx++] = c;
		} else {
			// Estouro: descarta a linha INTEIRA. Sem isto, a cauda de uma
			// linha longa seria interpretada como um pacote novo e valido.
			// O reject() so acontece no '\n', para contar uma vez por linha.
			discarding = true;
			bufferIdx  = 0;
		}
	}
}

void VirtualBridge::parsePacket(char* packet)
{
	char* sep = strchr(packet, ':');
	if (!sep) return reject(REJ_NO_SEP);

	*sep = '\0';
	const char* key    = packet;
	const char* valStr = sep + 1;

	// Chave precisa caber inteira. Truncar criaria colisao silenciosa:
	// "Temp1" e "Temp2" virariam a mesma entrada.
	size_t klen = strlen(key);
	if (klen == 0 || klen >= VB_KEY_MAX_LEN) return reject(REJ_BAD_KEY);

	// Valor precisa ser um numero completo, sem sobras. Sem esta checagem,
	// "Bp:erro" viraria 0.0 -- que na equacao barometrica da ~44330 m.
	char* end = NULL;
	double v = strtod(valStr, &end);
	if (end == valStr) return reject(REJ_BAD_VALUE);		// nada consumido
	while (*end == ' ' || *end == '\t') end++;				// tolera espaco final
	if (*end != '\0') return reject(REJ_BAD_VALUE);			// sobrou sujeira

	int8_t idx = findIndex(key);
	if (idx < 0) idx = addKey(key);
	if (idx < 0) return reject(REJ_TABLE_FULL);

	sensors[idx].value   = (float)v;
	sensors[idx].updated = true;
	if (++sensors[idx].seq == 0) sensors[idx].seq = 1;	// 0 fica reservado para "nunca recebeu"
}

int8_t VirtualBridge::findIndex(const char* key) const
{
	for (uint8_t i = 0; i < sensorCount; i++) {
		if (strcmp(sensors[i].key, key) == 0) return (int8_t)i;
	}
	return -1;
}

int8_t VirtualBridge::addKey(const char* key)
{
	if (sensorCount >= VB_MAX_SENSORS) return -1;
	strcpy(sensors[sensorCount].key, key);	// tamanho ja validado pelo chamador
	sensors[sensorCount].value   = 0.0f;
	sensors[sensorCount].seq     = 0;
	sensors[sensorCount].updated = false;
	return (int8_t)sensorCount++;
}

bool VirtualBridge::validHandle(int8_t h) const
{
	return (h >= 0) && (h < (int8_t)sensorCount);
}

int8_t VirtualBridge::handle(const char* key)
{
	if (key == NULL) return -1;
	size_t klen = strlen(key);
	if (klen == 0 || klen >= VB_KEY_MAX_LEN) return -1;

	int8_t idx = findIndex(key);
	if (idx >= 0) return idx;
	return addKey(key);		// permite resolver o handle antes do primeiro dado chegar
}

float VirtualBridge::value(int8_t h) const
{
	return validHandle(h) ? sensors[h].value : 0.0f;
}

bool VirtualBridge::hasData(int8_t h) const
{
	return validHandle(h) && (sensors[h].seq != 0);
}

uint16_t VirtualBridge::seq(int8_t h) const
{
	return validHandle(h) ? sensors[h].seq : 0;
}

bool VirtualBridge::isNew(int8_t h) const
{
	return validHandle(h) && sensors[h].updated;
}

void VirtualBridge::clearNew(int8_t h)
{
	if (validHandle(h)) sensors[h].updated = false;
}

const char* VirtualBridge::keyAt(int8_t h) const
{
	return validHandle(h) ? sensors[h].key : "";
}

float VirtualBridge::get(const char* key, float defaultVal)
{
	int8_t idx = findIndex(key);
	return (idx >= 0 && sensors[idx].seq != 0) ? sensors[idx].value : defaultVal;
}
