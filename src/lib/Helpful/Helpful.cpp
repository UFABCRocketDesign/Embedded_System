#include "Helpful.h"

///Coleaao de utilitarios
void Helpful::begin()
{
	T0 = micros();
}

unsigned long Helpful::counter()
{
	return count++;
}

unsigned long Helpful::getCount()
{
	return count;
}

unsigned long Helpful::counterReset()
{
	unsigned long C = count;
	count = 0;
	return  C;
}

bool Helpful::eachN(const unsigned int &N)
{
	conEach++;
	return (N == 0) ? false : conEach % N == 0;
}

bool Helpful::eachT(const float &T)
{
	if (micros() - timerEach > (unsigned long)(T*1000000.0))
	{
		timerEach = micros();
		return true;
	}
	return false;
}

void Helpful::forT(const float &T)
{
	endT = micros() + (long)(T*1000000.0);
	forTstate = 1;
}

bool Helpful::forT()
{
	if (micros() > endT && forTstate)
	{
		endT = 0;
		return forTstate = 0;
	}
	return forTstate;
}

float Helpful::lapse()
{
	unsigned long tnow = micros(), t = tnow - lapseT;
	lapseT = tnow;
	return (float)t / 1000000.0;
}

float Helpful::sinceBegin()
{
	return float(micros() - T0) / 1000000.0;
}

bool Helpful::oneTime()
{
	if (!one)
	{
		one = 1;
		return true;
	}
	return false;
}

void Helpful::oneTimeReset()
{
	if (one) one = 0;
}

void Helpful::comparer(const float &n)
{
	maxi = (n > maxi) ? n : maxi;
	mini = (n < mini) ? n : mini;
}

float Helpful::getMax()
{
	return maxi;
}

float Helpful::getMin()
{
	return mini;
}