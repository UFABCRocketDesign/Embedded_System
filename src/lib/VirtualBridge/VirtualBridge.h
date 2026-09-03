// VirtualBridge.h

// Recebe pela UART linhas no formato "Chave:valor" e mantem o ultimo valor
// de cada chave. Serve de fonte de dados para sensores simulados que herdam
// das classes reais (Sens, Baro, TriA...), permitindo testar a logica de voo
// com dados vindos do computador.
//
// Ha um objeto pronto, Bridge, definido no .cpp. Passe por referencia:

#pragma once

#ifndef _VIRTUAL_BRIDGE_h
#define _VIRTUAL_BRIDGE_h

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>

#define VB_MAX_SENSORS 16
#define VB_KEY_MAX_LEN  8	// 7 caracteres uteis + terminador
#define VB_BUFFER_LEN  48	// tamanho maximo de uma linha

// Analise de erros de recepcao (contador + motivo do ultimo descarte).
// Com 0, os membros somem e as chamadas de reject() viram nada.
// Defina antes de incluir este arquivo, ou por flag de build.
#ifndef VB_DIAGNOSTICS
#define VB_DIAGNOSTICS 1
#endif

// Motivo do ultimo pacote descartado. Combine com rejected():
// se a contagem nao cresce, o motivo e antigo.
enum RejectReason : uint8_t {
	REJ_NONE = 0,	// nenhum descarte ate agora
	REJ_NO_SEP,		// linha sem ':'
	REJ_BAD_KEY,	// chave vazia ou com mais de VB_KEY_MAX_LEN-1 caracteres
	REJ_BAD_VALUE,	// valor nao numerico ou com sujeira depois do numero
	REJ_OVERFLOW,	// linha maior que VB_BUFFER_LEN
	REJ_TABLE_FULL,	// ja ha VB_MAX_SENSORS chaves distintas
	REJ_DISABLED	// VB_DIAGNOSTICS == 0: nao ha analise de erros
};

class VirtualBridge {
	VirtualBridge(Stream &serialPort);
public:
	VirtualBridge(const VirtualBridge&) = delete;
	VirtualBridge& operator=(const VirtualBridge&) = delete;

	static VirtualBridge& getInstance(Stream &serialPort);

	// Deve ser chamado UMA vez por ciclo, antes de qualquer leitura, para
	// que todos os sensores enxerguem o mesmo instantaneo.
	void update();

	/////////////////// Caminho rapido: resolva o handle uma vez ///////////////////
	// handle() e O(n) e vai no begin(); value() e O(1) e vai no readAll().
	// O indice e estavel: chaves nunca sao removidas nem reordenadas.

	int8_t   handle(const char* key);	// registra se ainda nao existe; -1 se invalida ou tabela cheia
	float    value(int8_t h) const;		// O(1)
	bool     hasData(int8_t h) const;	// ja chegou algum valor para esta chave?
	uint16_t seq(int8_t h) const;		// contador monotonico; 0 = nunca recebeu
	bool     isNew(int8_t h) const;		// NAO consome
	void     clearNew(int8_t h);		// consome explicitamente

	/////////////////////// Caminho por nome: conveniencia ///////////////////////
	// O(n) em strcmp a cada chamada. Use nos sketches de teste, nao no laco de voo.

	float    get(const char* key, float defaultVal = 0.0f);

	//////////////////////////////// Diagnostico ////////////////////////////////
	// count()==0 e rejected()==0  -> nada chegando (cabo, baud, PC parado)
	// count()==0 e rejected()>0   -> chegando, mas o formato esta errado
	// rejected() crescendo        -> parte dos pacotes se perde
	// lastReject()==REJ_DISABLED  -> a analise foi compilada fora

	uint8_t     count() const { return sensorCount; }
	const char* keyAt(int8_t h) const;

#if VB_DIAGNOSTICS
	uint16_t     rejected() const { return rejectedCount; }
	RejectReason lastReject() const { return lastRejectReason; }
#else
	uint16_t     rejected() const { return 0; }
	RejectReason lastReject() const { return REJ_DISABLED; }
#endif

private:
	// Uma entrada da tabela. Detalhe interno: quem usa a ponte fala por
	// handle e pelos acessores, nunca com a estrutura diretamente.
	struct SensorData {
		char     key[VB_KEY_MAX_LEN];
		float    value;
		uint16_t seq;		// 0 = nunca recebeu dado; incrementa a cada atualizacao
		bool     updated;	// flag de conveniencia, limpa por clearNew()
	};

	Stream    &stream;
	SensorData sensors[VB_MAX_SENSORS];
	uint8_t    sensorCount;

	char       rxBuffer[VB_BUFFER_LEN];
	uint8_t    bufferIdx;
	bool       discarding;		// linha maior que o buffer: ignora ate o proximo '\n'

	// A condicional mora so aqui: os pontos de descarte no .cpp chamam
	// reject() sempre, e com VB_DIAGNOSTICS==0 a chamada some no inline.
#if VB_DIAGNOSTICS
	uint16_t     rejectedCount;
	RejectReason lastRejectReason;
	void reject(RejectReason r);
#else
	void reject(RejectReason) {}
#endif

	void   parsePacket(char* packet);
	int8_t findIndex(const char* key) const;
	int8_t addKey(const char* key);
	bool   validHandle(int8_t h) const;
};

////////////////////////////// Instancia do sistema //////////////////////////////
//
// Uma UART, uma ponte. Este e O objeto: declarado aqui, definido no .cpp.
// Nao crie outro, a menos que seja para teste.
//
// Para trocar de porta, edite VB_SERIAL_PORT aqui ou passe por flag de build.

#ifndef VB_SERIAL_PORT
#define VB_SERIAL_PORT Serial
#endif

extern VirtualBridge& Bridge;

#endif // _VIRTUAL_BRIDGE_h
