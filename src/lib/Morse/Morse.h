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

#ifndef MORSE_INTERRUPT
#define MORSE_INTERRUPT (0)
#endif // MORSE_INTERRUPT


#define _MORSE_MESSAGE_MAX_LENGTH 100
#define _MORSE_ALARM_STEPS 200

#define _MORSE_DEFAULT_ALARM_LOW 62
#define _MORSE_DEFAULT_ALARM_HIGH 1976

#define _MORSE_DEFAULT_ALARM_FAST 5
#define _MORSE_DEFAULT_ALARM_SLOW 100






#define ALARM_FUNC_(LOW, HIGH, STEP) \
(long(LOW) + long(HIGH - LOW) * ())


#define ALARM_FUNC_SIGMOID(STEP, WIDTH, ADJUST) \
(1.0/(1.0+exp(-(1.0/ADJUST)*(STEP/(WIDTH/2.0f)))))


#define ALARM_FUNC_TILDE(LOW, HIGH, STEP) \
(long(LOW) + long(HIGH - LOW) * (-cos(M_PI * STEP / 50.0) * 0.5 + 0.5))

#define ALARM_FUNC_HAT(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * (1- abs((2*STEP - float(WIDTH) - 1.0) / (float(WIDTH) - 1.0))))

#define ALARM_FUNC_LT(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * (STEP / float(WIDTH)))

#define ALARM_FUNC_GT(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * ((WIDTH - STEP) / float(WIDTH)))

#define ALARM_FUNC_NUM(LOW, HIGH, STEP) \
(((STEP / 50) % 2) ? LOW : HIGH)

#define ALARM_FUNC_LSQB(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * (pow((STEP - 1.0) / (float(WIDTH) - 1.0), 8)))

#define ALARM_FUNC_RSQB(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * (pow(1.0 - (STEP - 1.0) / (float(WIDTH) - 1.0), 3)))

#define ALARM_FUNC_LCUB(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * (1.0 - pow(1.0 - (STEP - 1.0) / (float(WIDTH) - 1.0), 3)))

#define ALARM_FUNC_RCUB(LOW, HIGH, STEP, WIDTH) \
(long(LOW) + long(HIGH - LOW) * (1.0 - pow((STEP - 1.0) / (float(WIDTH) - 1.0), 3)))

#define ALARM_FUNC_CIFRAO(LOW, HIGH, STEP) \
(long(LOW) + long(HIGH - LOW) * ((((STEP / 25) % 2) ? 0 : 1) - 1.5 * sin(M_PI * STEP / 25.0) * 0.5))

#define ALARM_FUNC_PERCENT(LOW, HIGH, STEP, ADJUST_A, ADJUST_B) \
(long(LOW) + long(HIGH - LOW) * ((sin(M_PI * STEP / 50.0) + sin(ADJUST_B*M_PI * STEP / 50.0)/ADJUST_A)/(1.0+1.0/ADJUST_A)+1) / 2.0)

#define ALARM_FUNC_PIPE(LOW, HIGH, STEP, ADJUST) \
(long(LOW) + long(HIGH - LOW) * (1+sin(M_PI * STEP / 50.0)*(sin(ADJUST * M_PI * STEP / 50.0))))

#define ALARM_FUNC_UNDER(LOW, HIGH, STEP, ADJUST) \
(long(LOW) + long(HIGH - LOW) * ((((STEP / 50) % 2) ? -1 : 1)*(1/(2+cos(M_PI+ADJUST * M_PI * STEP / 50.0)))+1))




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
		uint16_t cifrao[_MORSE_ALARM_STEPS]; // $
		uint16_t percent[_MORSE_ALARM_STEPS]; // %
		uint16_t pipe[_MORSE_ALARM_STEPS]; // |
		uint16_t under[_MORSE_ALARM_STEPS]; // _
	};

	// Função que calcula os valores (roda apenas no seu PC durante o upload)
	static constexpr AlarmTable generateAlarmTable() {
		AlarmTable table = {};

		const int ALARM_LOW = _MORSE_DEFAULT_ALARM_LOW;
		const int ALARM_HIGH = _MORSE_DEFAULT_ALARM_HIGH;
		const int alarmSteps = _MORSE_ALARM_STEPS;

		for (int currentMarkIndex = 0; currentMarkIndex < alarmSteps; currentMarkIndex++) {
			table.tilde[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_TILDE(ALARM_LOW, ALARM_HIGH, currentMarkIndex));
			table.hat[currentMarkIndex]		= (uint16_t)(ALARM_FUNC_HAT(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.lt[currentMarkIndex]		= (uint16_t)(ALARM_FUNC_LT(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.num[currentMarkIndex]		= (uint16_t)(ALARM_FUNC_NUM(ALARM_LOW, ALARM_HIGH, currentMarkIndex));
			table.gt[currentMarkIndex]		= (uint16_t)(ALARM_FUNC_GT(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.lsqb[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_LSQB(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.rsqb[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_RSQB(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.lcub[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_LCUB(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.rcub[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_RCUB(ALARM_LOW, ALARM_HIGH, currentMarkIndex, alarmSteps));
			table.cifrao[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_CIFRAO(ALARM_LOW, ALARM_HIGH, currentMarkIndex));
			table.percent[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_PERCENT(ALARM_LOW, ALARM_HIGH, currentMarkIndex, 2.0, 5));
			table.pipe[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_PIPE(ALARM_LOW, ALARM_HIGH, currentMarkIndex, 10));
			table.under[currentMarkIndex]	= (uint16_t)(ALARM_FUNC_UNDER(ALARM_LOW, ALARM_HIGH, currentMarkIndex, 6));
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
	const unsigned int ALARM_LOW = _MORSE_DEFAULT_ALARM_LOW;
	const unsigned int ALARM_HIGH = _MORSE_DEFAULT_ALARM_HIGH;

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
	void _MORSE_FUNCTION_ATTR playMorseAlarm(long currentMillis);
	virtual void _MORSE_FUNCTION_ATTR alarmAppendix(unsigned int signal, long currentMillis);

public:

	String msgAux = "";

	Morse(uint8_t pin, String msg = "", unsigned int fDot = 523, unsigned int fDash = 784, unsigned int fAlrmHi = _MORSE_DEFAULT_ALARM_LOW, unsigned int fAlrmLo = _MORSE_DEFAULT_ALARM_HIGH);
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
	void _MORSE_FUNCTION_ATTR alarmAppendix(unsigned int signal, long currentMillis);

	public:
	MorseAtvBzz(uint8_t pin, bool cmd = LOW, String msg = "", unsigned int fAlrmFs = _MORSE_DEFAULT_ALARM_FAST, unsigned int fAlrmSl = _MORSE_DEFAULT_ALARM_SLOW);
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

#if MORSE_INTERRUPT

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

	extern hw_timer_t * _morseTimer;


#endif // defined(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_AVR)




#if defined(ARDUINO_ARCH_AVR)

#define MORSE_INTERRPUT_PRESET(MORSE_OBJECT) \
ISR(TIMERn_COMPA_vect) { \
	cli(); \
	MORSE_OBJECT.updateMorse(); \
	sei(); \
}

#elif defined(ARDUINO_ARCH_ESP32)

void ARDUINO_ISR_ATTR InterruptServiceRoutine();

#define MORSE_INTERRPUT_PRESET(MORSE_OBJECT) \
void ARDUINO_ISR_ATTR InterruptServiceRoutine() { \
	MORSE_OBJECT.updateMorse(); \
}

#endif // define(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_ESP32)

void morseInterruptionSetup();

#endif // MORSE_INTERRUPT

#endif