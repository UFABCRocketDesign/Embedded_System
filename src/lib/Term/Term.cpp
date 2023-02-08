#include "Term.h"

///Termometro
Term::Term(byte aPin) :Apin(aPin)
{
}

float Term::read()
{
	return (float(analogRead(Apin)) * 5 / (1023)) / 0.01;
}