// Morse.h

#pragma once

#ifndef _MORSE_h
#define _MORSE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// No AVR, usamos PROGMEM. No ESP32, usamos DRAM_ATTR.
#if defined(ARDUINO_ARCH_AVR)
#define _MORSE_MEMORY_ATTR PROGMEM
#elif defined(ARDUINO_ARCH_ESP32)
#define _MORSE_MEMORY_ATTR DRAM_ATTR
#else
#define _MORSE_MEMORY_ATTR
#endif

#if defined(ARDUINO_ARCH_ESP32)
#define _MORSE_FUNCTION_ATTR IRAM_ATTR
#else
#define _MORSE_FUNCTION_ATTR
#endif


#if !defined(_MORSE_PRINT)
#define _MORSE_PRINT 0
#endif // _MORSE_PRINT

#if !defined(_MORSE_INTERRUPT)
#define _MORSE_INTERRUPT 0
#endif // _MORSE_INTERRUPT


#define _MORSE_MESSAGE_MAX_LENGTH 100
#define _MORSE_ALARM_STEPS 200

#if defined(ARDUINO_ARCH_ESP32)
namespace MorseAlarm {
	struct AlarmTable {
		uint16_t tilde[_MORSE_ALARM_STEPS]; // ~
		uint16_t hat[_MORSE_ALARM_STEPS]; // ^
		uint16_t lt[_MORSE_ALARM_STEPS]; // <
		uint16_t num[_MORSE_ALARM_STEPS]; // #
		uint16_t gt[_MORSE_ALARM_STEPS]; // >
		uint16_t lsqb[_MORSE_ALARM_STEPS]; // [
		uint16_t rsqb[_MORSE_ALARM_STEPS]; // ]
		uint16_t lcub[_MORSE_ALARM_STEPS]; // {
		uint16_t rcub[_MORSE_ALARM_STEPS]; // }
	};

	// Função que calcula os valores (roda apenas no seu PC durante o upload)
	static constexpr AlarmTable generateAlarmTable() {
		AlarmTable table = {};

		const int ALARM_LOW = 62;
		const int ALARM_HIGH = 1976;
		const int alarmSteps = _MORSE_ALARM_STEPS;

		for (int currentMarkIndex = 0; currentMarkIndex < alarmSteps; currentMarkIndex++) {
			table.tilde[currentMarkIndex]	= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (-cos(M_PI * currentMarkIndex / 50.0) * 0.5 + 0.5));
			table.hat[currentMarkIndex]		= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1- abs((2*currentMarkIndex - float(alarmSteps) - 1.0) / (float(alarmSteps) - 1.0))));
			table.lt[currentMarkIndex]		= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (currentMarkIndex / float(alarmSteps)));
			table.num[currentMarkIndex]		= (int16_t)(((currentMarkIndex / 50) % 2) ? ALARM_LOW : ALARM_HIGH);
			table.gt[currentMarkIndex]		= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * ((alarmSteps - currentMarkIndex) / float(alarmSteps)));
			table.lsqb[currentMarkIndex]	= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8)));
			table.rsqb[currentMarkIndex]	= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8)));
			table.lcub[currentMarkIndex]	= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8)));
			table.rcub[currentMarkIndex]	= (int16_t)(long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8)));
		}
		return table;
	}
}
#endif // defined(ARDUINO_ARCH_ESP32)

class Morse {

protected:

	#if defined(ARDUINO_ARCH_ESP32)
	// Isso força o compilador a gravar os dados na memória de dados (DRAM_ATTR)
	static constexpr MorseAlarm::AlarmTable greatAlarmTable _MORSE_MEMORY_ATTR = MorseAlarm::generateAlarmTable();
	#endif // defined(ARDUINO_ARCH_ESP32)

	static const char *const letters[26];  //For Letters
	static const char *const numbers[10];  //For Numbers
	static const char *const sign1[16];     //For Alarm
	static const char *const sign2[7];   //For Alarm
	static const char *const sign3[6];    //For Alarm
	static const char *const sign4[4];     //For Alarm

	const int alarmSteps = _MORSE_ALARM_STEPS;

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
	char currentMark = 0x16; // char SYN = 0x16 (	Synchronous Idle ASCII char)
	long nextActionMillis = 0;
	char currentMessage[_MORSE_MESSAGE_MAX_LENGTH] = {};
	// String currentMessage = "";
	char nextMessage[_MORSE_MESSAGE_MAX_LENGTH] = {};
	// String nextMessage = "";
	bool newMessage = false;
	bool quiet = false;
	bool messageComplete = false; // verdadeiro após a mensagem ser concluida pelo menos uma vez, setQuiet() resseta este valor

	char const* _MORSE_FUNCTION_ATTR selectSeq(char c);
	virtual bool _MORSE_FUNCTION_ATTR playMorseChar(char c);

public:

	String msgAux = "";

	Morse(uint8_t pin, String msg = "", unsigned int fDot = 523, unsigned int fDash = 784, unsigned int fAlrmHi = 62, unsigned int fAlrmLo = 1976);
	virtual void setup(); // Inicializa pinos
	void setNextMessage(String message); // Configura a próxima mensagem (só vai tocar quando a atual acabar)
	bool _MORSE_FUNCTION_ATTR updateMorse(); // Toca a mensagem e retorna verdadeiro toda vez que acaba
	virtual void setQuiet(); // Silencia o sistema
	void unsetQuiet(); // Reativa o sistema
	bool getQuiet(); // Verifica se o sistema está silenciado
	bool getMessageComplete(); // Retorna verdadeiro após a mensagem ser concluida pelo menos uma vez, setQuiet() resseta este valor
};

class MorseAtvBzz : public Morse
{
	const uint8_t buzzerCmd;

	uint8_t alarmState = !buzzerCmd;

	bool _MORSE_FUNCTION_ATTR playMorseChar(char c);

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

#if _MORSE_INTERRUPT

#if defined(ARDUINO_ARCH_AVR)


	#define TIMERn_COMPA_vect TIMER3_COMPA_vect
	#define TCCRnA TCCR3A
	#define TCCRnB TCCR3B
	#define TCNTn TCNT3
	#define OCRnA OCR3A
	#define TIMSKn TIMSK3

	#define WGMn2 WGM32
	#define OCIEnA OCIE3A


	/*
	0b001 -> 1 -> 2^0 -> 1
	0b010 -> 2 -> 2^3 -> 8
	0b011 -> 3 -> 2^6 -> 64
	0b100 -> 4 -> 2^8 -> 256
	0b101 -> 5 -> 2^10 -> 1024
	*/
	#define CSnB_BITS 0b010

	#if (CSnB_BITS == 0b001)
	#define PRESCALERnB 1
	#elif (CSnB_BITS == 0b010)
	#define PRESCALERnB 8
	#else
	#define PRESCALERnB (1 << ((CSnB_BITS) << 1))
	#endif  // CSnB_BITS

	#define TCCRnB_SET (0x00 | (CSnB_BITS))

	#define F_INTERRUPT 200

	#define TCNTn_VALUE uint16_t(((uint32_t(F_CPU) / uint32_t(F_INTERRUPT)) / (uint32_t(PRESCALERnB)))-1)

#elif defined(ARDUINO_ARCH_ESP32)

	#define TIMER_INTERVAL_MS 5

	hw_timer_t * _morseTimer = NULL;


#endif // defined(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_AVR)




#if defined(ARDUINO_ARCH_AVR)

#define MORSE_INTERRPUT_PRESET(MORSE_OBJECT) \
ISR(TIMERn_COMPA_vect) { \
	cli(); \
	MORSE_OBJECT.updateMorse(); \
	sei(); \
}

#elif defined(ARDUINO_ARCH_ESP32)

#define MORSE_INTERRPUT_PRESET(MORSE_OBJECT) \
void ARDUINO_ISR_ATTR InterruptServiceRoutine() { \
	MORSE_OBJECT.updateMorse(); \
}

#endif // define(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_ESP32)


void morseInterruptionSetup()
{
	#if _MORSE_PRINT
	Serial.println("ISR config ini");
	#endif // _MORSE_PRINT

	#if defined(ARDUINO_ARCH_AVR)
	cli();  // Disable interrupts

	// Configure Timer3 for CTC mode
	TCCRnA = 0; // Clear TCCR3A register
	TCCRnB = 0; // Clear TCCR3B register

	// Set CTC mode (Clear Timer on Compare Match)
	TCCRnB |= (1 << WGMn2);
	// Set prescaler to 8
	TCCRnB |= TCCRnB_SET;
	// TCCR3B |= (1 << CS31);

	TCNTn = 0; // Initialize timer counter to 0

	// Calculate OCR3A for 50Hz interrupt (16MHz / (50Hz * 8 prescaler) - 1)
	OCRnA = TCNTn_VALUE;
	// OCR3A = 39999;

	// Enable Timer3 Compare Match A interrupt
	TIMSKn |= (1 << OCIEnA);

	sei();  // Enable interrupts
	#elif defined(ARDUINO_ARCH_ESP32)

	extern void InterruptServiceRoutine();

	_morseTimer = timerBegin(1000000);
	timerAttachInterrupt(_morseTimer, &InterruptServiceRoutine);
	timerAlarm(_morseTimer, TIMER_INTERVAL_MS * 1000, true, 0);

	#endif // define(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_ESP32)

	#if _MORSE_PRINT
	Serial.println("ISR config fin");
	#endif // _MORSE_PRINT
}

#endif // _MORSE_INTERRUPT

#endif