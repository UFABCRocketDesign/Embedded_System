#include "DuDeploy.h"

///Controle de paraquedas
DuDeploy::DuDeploy(unsigned int paraPin1, unsigned int paraPin2, unsigned int infPin1, unsigned int infPin2, float ignT, float delay, float tMax) : P1(paraPin1), P2(paraPin2), I1(infPin1), I2(infPin2), Tign((unsigned long)(ignT * 1000000.0)), Delay((unsigned long)(delay * 1000000.0)), Tmax((unsigned long)(tMax * 1000000.0)), TmaxAux(tMax == 0 ? 0 : 1)
{
}

void DuDeploy::resetTimer()
{
	TimeZero = micros();
}

bool DuDeploy::info1()
{
	return digitalRead(I1);
}

bool DuDeploy::info2()
{
	return digitalRead(I2);
}

bool DuDeploy::begin()
{
	pinMode(P1, OUTPUT);
	pinMode(P2, OUTPUT);
	digitalWrite(P1, LOW);
	digitalWrite(P2, LOW);
	pinMode(I1, INPUT_PULLUP);
	pinMode(I2, INPUT_PULLUP);
	resetTimer();
	return info1() || info2();
}

void DuDeploy::setTmax(float Time)
{
	Tmax = (unsigned long)(Time * 1000000);
	TmaxAux = true;
}

void DuDeploy::setTdelay(float Time)
{
	TdelayAux = true;
	Tdelay = (unsigned long)(Time * 1000000);
}

void DuDeploy::setP1height(float H)
{
	P1H = H;
	P1H_A = true;
}

void DuDeploy::setP2height(float H)
{
	P2H = H;
	P2H_A = true;
}

void DuDeploy::sealApogee(bool apg)
{
	if (!apogee)
	{
		Tseal = micros() - TimeZero;
		apogee = apg;
	}
}

bool DuDeploy::getApogee()
{
	return apogee;
}

bool DuDeploy::getP1S(bool type)
{
	if (type) return P1S;
	else
	{
		if (!P1S_A && P1S)
		{
			P1S_A = 1;
			return 1;
		}
		else return 0;
	}
}

bool DuDeploy::getP2S(bool type)
{
	if (type) return P2S;
	else
	{
		if (!P2S_A && P2S)
		{
			P2S_A = 1;
			return 1;
		}
		else return 0;
	}
}

bool DuDeploy::getSysState(bool type)
{
	return (P1T_A && P2T_A && (P1S || P2S)) || (P2T_A && !P1S && (!P1T_A || P2S)) || (P1T_A && !P2S && (P1S || !P2T_A)) || (!type && !P1S && !P2S && (!P1T_A || !P2T_A));
}

void DuDeploy::refresh(float height)
{
	Tnow = micros() - TimeZero;
	if (TmaxAux && Tnow > Tmax && !apogee) sealApogee(true);	//Caso o tempo maximo seja exigido e tenha sido ultrapassado
	if (apogee && getSysState(0))	//Se o apogeu foi dado
	{
		if (TdelayAux)	//Somente caso o uso do atraso seja definido
		{
			if (!P1M_A)	//Salva somente uma vez
			{
				if (!P1H_A || (P1H_A && height <= P1H))	//Condicional base P1
				{
					P1M = Tnow;	//Momento de condicional verdadeira
					P1M_A = true;
				}
			}
			/*if (!P2M_A) //Salva somente uma vez
			{
				if ((!P2H_A && Tnow > P1T + Delay) || (P2H_A && height <= P2H))	//Condicional base P2
				{
					P2M = Tnow;	//Momento de condicional verdadeira
					P2M_A = true;
				}
			}*/
		}
		if (P1seal || (!P1H_A || (P1H_A && height <= P1H)) && !TdelayAux || TdelayAux && Tnow > P1M + Tdelay)	//Caso altura seja exigida, sera verificada
		{
			if (!P1seal) P1seal = 1;
			if (!P1T_A)	//Salvar tempo do acionamento de P1 somente uma vez
			{
				P1T_A = 1;
				P1T = Tnow;
			}
			if ((Tnow - P1T < Tign) && P1T_A) P1S = 1;	//Comando efetivo
			else P1S = 0;

		}
		if (P2seal || ((!P2H_A && Tnow > P1T + Delay) || (P2H_A && height <= P2H)) /* && !TdelayAux || TdelayAux && Tnow > P2M + Tdelay*/)	//Verifica tempo de atraso ou altura exigida
		{
			if (!P2seal) P2seal = 1;
			if (!P2T_A)	//Salva tempo do acionamento de P2 somente uma vez
			{
				P2T_A = 1;
				P2T = Tnow;
			}
			if (Tnow < P2T + Tign && P2T_A) P2S = 1;	//Comando efetivo
			else P2S = 0;
		}
	}
	digitalWrite(P1, P1S);
	digitalWrite(P2, P2S);
}

void DuDeploy::emergency(bool state)
{
	if (state && !emer)
	{
		P1H_Am = P1H_A;
		P2H_Am = P2H_A;
		P1H_A = false;
		P2H_A = false;
		sealApogee(true);
	}
	if (!state && emer)
	{
		P1H_A = P1H_Am;
		P2H_A = P2H_Am;
	}
	emer = state;
}