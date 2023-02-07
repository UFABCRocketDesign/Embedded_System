// DuDeploy.h

#pragma once

#ifndef _DUDEPLOY_h
#define _DUDEPLOY_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class DuDeploy
{
	const unsigned int P1, P2;		// Pinos de comando
	const unsigned int I1, I2;		// Pinos de verificacao
	const unsigned long Tign;		// Tempo de comando ativo
	const unsigned long Delay;		// Tempo de atraso dentre comando (caso altura nao determinada)
	unsigned long P1T = 0, P2T = 0; // Momento que o ignitor ligou
	unsigned long TimeZero = 0;		// Momento zero
	unsigned long Tmax;				// Tempo maximo para segunranca
	unsigned long Tseal;			// Tempo onde o selo aconteceu
	unsigned long Tdelay;			// Tempo de atraso configurado
	unsigned long P1M, P2M;			// Tempo onde a condicional do paraquedas foi atingida
	unsigned long Tnow = 0;			// Tempo atual (com zeragem)
	float P1H = 0, P2H = 0;			// Altura de comando
	bool P1S = 0, P2S = 0;			// Estado do acionamento
	bool P1S_A = 0, P2S_A = 0;		// Estado atual do comando
	bool P1H_A = 0, P2H_A = 0;		// Condicional para comando em altura
	bool apogee = 0;				// Representante interno do apogeu
	bool TmaxAux;					// Condicional para verificar tempo de seguranca
	bool TdelayAux = 0;				// Condicional para verificar modo de atraso
	bool P1M_A = 0, P2M_A = 0;		// Condicional do paraquedas foi atingida
	bool P1T_A = 0, P2T_A = 0;		// Condicional para momento do ignitor
	bool P1seal = 0, P2seal = 0;	// Selo de comando
	bool emer = 0, P1H_Am = 0, P2H_Am = 0;

public:
	DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay, float tMax = 0);
	void resetTimer();
	bool info1(); // Retorna informacao sobre estado do ignitor de acionamento 1
	bool info2(); // Retorna informacao sobre estado do ignitor de acionamento 2
	bool begin();
	void setTmax(float Time);
	void setTdelay(float Time);
	void setP1height(float H);
	void setP2height(float H);
	void sealApogee(bool apg);
	bool getApogee();
	bool getP1S(bool type = 1);
	bool getP2S(bool type = 1);
	bool getSysState(bool type = 1);
	void refresh(float height);
	void emergency(bool state = 1);
};

#endif
