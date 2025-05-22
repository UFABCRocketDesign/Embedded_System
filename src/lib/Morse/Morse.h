// Morse.h

#pragma once

#ifndef _MORSE_h
#define _MORSE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef _MORSE_PRINT
#define _MORSE_PRINT 0
#endif // _MORSE_PRINT

class Morse {

protected:

	static const char *const letters[26];  //For Letters
	static const char *const numbers[10];  //For Numbers
	static const char *const space[1];     //For Space
	static const char *const morse[2];     //For Morse
	static const char *const tilde[4];     //For Alarm
	static const char *const zigzag[5];    //For Alarm
	static const char *const compare[3];   //For Alarm

	const int alarmSteps = 400;

	const unsigned int FREQ_DOT = 523;
	const unsigned int FREQ_DASH = 784;
	const unsigned int ALARM_LOW = 62;
	const unsigned int ALARM_HIGH = 1976;

	const unsigned int dotDelay = 150;
	const unsigned int dashDelay = 3 * dotDelay;
	const unsigned int alarmDelay = 5;

	const uint8_t buzzerPin;

	int currentCharIndex = 0;
	int currentMarkIndex = 0;
	char currentMark = ' ';
	long nextActionMillis = 0;
	String currentMessage = "";
	String nextMessage = "";
	bool newMessage = false;
	bool quiet = false;

	virtual bool playMorseChar(char c);

public:

	String msgAux = "";

	Morse(uint8_t pin, String msg = "", unsigned int fDot = 523, unsigned int fDash = 784, unsigned int fAlrmHi = 62, unsigned int fAlrmLo = 1976);
	virtual void setup(); // Inicializa pinos
	void setNextMessage(String message); // Configura a próxima mensagem (só vai tocar quando a atual acabar)
	bool updateMorse(); // Toca a mensagem e retorna verdadeiro toda vez que acaba
	virtual void setQuiet(); // Silencia o sistema
	bool getQuiet(); // Verifica se o sistema está silenciado
};

class MorseAtvBzz : public Morse
{
	const uint8_t buzzerCmd;

	uint8_t alarmState = !buzzerCmd;

	bool playMorseChar(char c);

	public:
	MorseAtvBzz(uint8_t pin, bool cmd = LOW, String msg = "", unsigned int fAlrmFs = 5, unsigned int fAlrmSl = 100);
	void setup();
	void setQuiet();
};

/*
T	-
M	--
O	---
0	-----
9	----.
8	---..
G	--.
Q	--.-
Z	--..
7	--...
N	-.
K	-.-
Y	-.--
C	-.-.
D	-..
X	-..-
B	-...
6	-....
E	.
A	.-
W	.--
J	.---
1	.----
P	.--.
R	.-.
L	.-..
I	..
U	..-
2	..---
F	..-.
S	...
V	...-
3	...--
H	....
4	....-
5	.....
*/

#endif