#include "SDCH.h"

///Auxiliar para o uso do cartao SD
SDCH::SDCH(uint8_t csPin, String name, String type) :CS(csPin), Fname0(name), Ftype((type.length() == 3) ? type : (String)"txt"), coef(8 - name.length()), nMax(pow(10, (name.length() < 8) ? 8 - name.length() : 0))
{
	newName();
}
void SDCH::newName()
{
	Fname = Fname0;
	if (coef > 0)
	{
		if (String(number).length() > coef) number = 0;
		String n = String(number);
		Fname = Fname0;
		for (int i = coef - n.length(); i > 0; i--)	Fname += '0';
		Fname += n;
		number++;
	}
	else
	{
		String aux = Fname;
		Fname = "";
		for (int i = 0; i < 8; i++) Fname += aux[i];
	}
	Fname += '.' + Ftype;
}
bool SDCH::begin()
{
	if (!SD.begin(CS)) return false;
	else
	{ //Inicializa o SD
		for (unsigned long int i = 0; i < nMax; i++)
		{ //Cria um arquivo com nome novo
			if (!SD.exists(Fname)) break;
			else newName();
		}
		theFile = SD.open(Fname, FILE_WRITE);
		if (theFile)
		{
			theFile.close();
			return true;
		}
		else return false;
	}
}
bool SDCH::open()
{
	return theFile = SD.open(Fname, FILE_WRITE);
}
size_t SDCH::tab()
{
	return theFile.write('\t');
}
void SDCH::close()
{
	theFile.close();
}
SDCH::operator bool()
{
	return open();
}
String SDCH::getFname()
{
	return Fname;
}