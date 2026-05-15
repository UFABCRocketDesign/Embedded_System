#include "Morse.h"

// For letters
const char *const Morse::letters[] _MORSE_MEMORY_ATTR = {
		".-",	// A
		"-...",	// B
		"-.-.",	// C
		"-..",	// D
		".",	// E
		"..-.",	// F
		"--.",	// G
		"....",	// H
		"..",	// I
		".---",	// J
		"-.-",	// K
		".-..",	// L
		"--",	// M
		"-.",	// N
		"---",	// O
		".--.",	// P
		"--.-",	// Q
		".-.",	// R
		"...",	// S
		"-",	// T
		"..-",	// U
		"...-",	// V
		".--",	// W
		"-..-",	// X
		"-.--",	// Y
		"--.." 	// Z
	};

// For Numbers
const char *const Morse::numbers[] _MORSE_MEMORY_ATTR = {
		"-----", // 0
		".----", // 1
		"..---", // 2
		"...--", // 3
		"....-", // 4
		".....", // 5
		"-....", // 6
		"--...", // 7
		"---..", // 8
		"----."  // 9
	};

// For Alarms
const char *const Morse::sign1[] _MORSE_MEMORY_ATTR = {
		" ", //
		"-.-.--", // !
		".-..-.", // "
		"#", // #
		"%", // %
		"$", // $
		".-...", // &
		".----.", // '
		"-.--.", // (
		"-.--.-", // )
		"*", // *
		".-.-.", // +
		"--,,--", // ,
		"-", // -
		".", // .
		"-..-."  // /
	};

const char *const Morse::sign2[] _MORSE_MEMORY_ATTR = {
		"---...",	// :
		"-.-.-.",	// ;
		"<",	// <
		"-...-",	// =
		">",	// >
		"..--..",	// ?
		".---." 	// @
	};

const char *const Morse::sign3[] _MORSE_MEMORY_ATTR = {
		"[",	// [
		"\\",	// "\"
		"]",	// ]
		"^",	// ^
		"_",	// _
		"`" 	// `
	};

const char *const Morse::sign4[] _MORSE_MEMORY_ATTR = {
		"{",	// {
		"|",	// |
		"}",	// }
		"~" 	// ~
	};


Morse::Morse(uint8_t pin, String msg, unsigned int fDot, unsigned int fDash, unsigned int fAlrmHi, unsigned int fAlrmLo)
	: buzzerPin(pin),
	//   currentMessage(msg),
	  FREQ_DOT(fDot),
	  FREQ_DASH(fDash),
	  ALARM_LOW(fAlrmHi),
	  ALARM_HIGH(fAlrmLo) {
		msg.toCharArray(currentMessage, _MORSE_MESSAGE_MAX_LENGTH);
	  }

void Morse::setup()
{
	#if defined(ARDUINO_ARCH_AVR)
	pinMode(buzzerPin, OUTPUT);
	#elif defined(ARDUINO_ARCH_ESP32)
	ledcAttach(buzzerPin, 2000, 8);
	#endif
}

void Morse::setNextMessage(String message)
{
	// nextMessage = message;
	message.toCharArray(nextMessage, _MORSE_MESSAGE_MAX_LENGTH);
	newMessage = true;
}

void Morse::setQuiet()
{
	messageComplete = false;
	#if defined(ARDUINO_ARCH_AVR)
	noTone(buzzerPin);
	#elif defined(ARDUINO_ARCH_ESP32)
	ledcWriteTone(buzzerPin, 0);
	#endif
	currentCharIndex = 0;
	currentMarkIndex = 0;
	quiet = true;

	#if _MORSE_INTERRUPT

	#if defined(ARDUINO_ARCH_AVR)
	TIMSKn &= ~(1 << OCIEnA);
	#elif defined(ARDUINO_ARCH_ESP32)
	timerAlarmDisable(_morseTimer);
	#endif // defined(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_AVR)

	#endif // _MORSE_INTERRUPT
}

void Morse::unsetQuiet()
{
	#if _MORSE_INTERRUPT
	if(quiet){
		#if defined(ARDUINO_ARCH_AVR)
		TIMSKn |= (1 << OCIEnA);
		#elif defined(ARDUINO_ARCH_ESP32)
		timerAlarmEnable(_morseTimer);
		#endif // defined(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_AVR)
	}
	#endif // _MORSE_INTERRUPT
	// ets_printf("updateMorse0\n");
	quiet = false;
}

bool Morse::getQuiet()
{
	return quiet;
}

bool Morse::getMessageComplete()
{
	return messageComplete;
}

char const* Morse::selectSeq(char c){
	char const *seq = 0;

	if (c >= '0' && c<='9')
		// seq = Morse::numbers[c - '0'];
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::numbers + (c - '0'))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::numbers[c - '0']
			#endif // defined(ARDUINO_ARCH_AVR)
				);
	else if (c>= 'A' && c<= 'Z')
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::letters + (c - 'A'))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::letters[c - 'A']
			#endif // defined(ARDUINO_ARCH_AVR)
		);
	else if (c>= 'a' && c<= 'z')
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::letters + (c - 'a'))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::letters[c - 'a']
			#endif // defined(ARDUINO_ARCH_AVR)
		);
	else if (c >= ' ' && c <= '/')
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::sign1 + (c - ' '))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::sign1[c - ' ']
			#endif // defined(ARDUINO_ARCH_AVR)
		);
	else if (c >= '{' && c <= '~')
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::sign4 + (c - '{'))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::sign4[c - '{']
			#endif // defined(ARDUINO_ARCH_AVR)
		);
	else if (c >= '[' && c <= '`')
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::sign3 + (c - '['))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::sign3[c - '[']
			#endif // defined(ARDUINO_ARCH_AVR)
		);
	else if (c >= ':' && c <= '@')
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::sign2 + (c - ':'))
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::sign2[c - ':']
			#endif // defined(ARDUINO_ARCH_AVR)
		);
	else
		seq = (const char*)(
			#if defined(ARDUINO_ARCH_AVR)
			pgm_read_ptr_near(Morse::sign1 + 0)
			#elif defined(ARDUINO_ARCH_ESP32)
			Morse::sign1[0]
			#endif // defined(ARDUINO_ARCH_AVR)
		);

	return seq;
}

bool Morse::playMorseChar(char c)
{
	volatile char const *seq = selectSeq(c);
	bool numAux = (c >= '0' && c<='9');
	long currentMillis = millis();

	if (currentMillis >= nextActionMillis)
	{
		// ets_printf("%d\t%c\t%d\n",currentMarkIndex, currentMark, currentMark);
		// Serial.println((unsigned int)seq);
		if (currentMark == '\0')
		{
			currentMarkIndex = 0;
			currentMark = 0x16;
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			nextActionMillis = currentMillis + dotDelay;
			return 1;
		}

		if (currentMark == 0x16)
		{
			#if defined(ARDUINO_ARCH_AVR)
			noTone(buzzerPin);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, 0);
			#endif
			currentMark = seq[currentMarkIndex++];
			nextActionMillis = currentMillis + dotDelay;
		}
		else if (currentMark == '.')
		{
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, FREQ_DASH * (numAux ? 0.5 : 1));
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, FREQ_DASH * (numAux ? 0.5 : 1));
			#endif
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			currentMark = 0x16;
			nextActionMillis = currentMillis + dotDelay;
		}
		else if (currentMark == '-')
		{
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, FREQ_DOT * (numAux ? 0.5 : 1));
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, FREQ_DOT * (numAux ? 0.5 : 1));
			#endif
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			currentMark = 0x16;
			nextActionMillis = currentMillis + dashDelay;
		}
		else if (currentMark == ' ')
		{
			#if defined(ARDUINO_ARCH_AVR)
			noTone(buzzerPin);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, 0);
			#endif
			#if _MORSE_PRINT
			Serial.print(0x16);
			#endif // _MORSE_PRINT
			currentMark = 0x16;
			nextActionMillis = currentMillis + dashDelay;
		}
		else if (currentMark == '~')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (-cos(M_PI * currentMarkIndex / 50.0) * 0.5 + 0.5);
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.tilde[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '^')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1- abs((2*currentMarkIndex - float(alarmSteps) - 1.0) / (float(alarmSteps) - 1.0)));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.hat[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '<')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (currentMarkIndex / float(alarmSteps));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.lt[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '#')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = ((currentMarkIndex / 50) % 2) ? ALARM_LOW : ALARM_HIGH;
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.num[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '>')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * ((alarmSteps - currentMarkIndex) / float(alarmSteps));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.gt[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '[')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.lsqb[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == ']')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.rsqb[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '{')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.lcub[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else if (currentMark == '}')
		{
			nextActionMillis = currentMillis + alarmDelay;

			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.rcub[currentMarkIndex % alarmSteps];
			#endif

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			#if defined(ARDUINO_ARCH_AVR)
			tone(buzzerPin, signal);
			#elif defined(ARDUINO_ARCH_ESP32)
			ledcWriteTone(buzzerPin, signal);
			#endif
		}
		else
			currentMark = '\0';
	}
	return 0;
}

bool Morse::updateMorse()
{
	// #if _MORSE_INTERRUPT
	// if(quiet){
	// 	#if defined(ARDUINO_ARCH_AVR)
	// 	TIMSKn |= (1 << OCIEnA);
	// 	#elif defined(ARDUINO_ARCH_ESP32)
	// 	timerAlarmEnable(_morseTimer);
	// 	#endif // defined(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_AVR)
	// }
	// #endif // _MORSE_INTERRUPT
	// // ets_printf("updateMorse0\n");
	// quiet = false;
	unsetQuiet();
	if (currentCharIndex == 0 && newMessage)
	{
		// currentMessage = nextMessage;
		strncpy(currentMessage, nextMessage, sizeof(currentMessage));
		newMessage = false;
	}
	// if (currentCharIndex <= currentMessage.length())
	if (currentMessage[currentCharIndex] != '\0')
	{
		byte ret = playMorseChar(currentMessage[currentCharIndex]);
		// ets_printf("updateMorse1\n");
		if (ret)
		{
			currentMark = 0x16;
        	currentMarkIndex = 0;
			currentCharIndex++;
		}
	}
	else
	{
		currentCharIndex = 0;
		messageComplete = true;
		#if _MORSE_PRINT
		Serial.println();
		#endif // _MORSE_PRINT
		// ets_printf("updateMorse8\n");
		return true;
	}
	// ets_printf("updateMorse9\n");
	return false;
}


MorseAtvBzz::MorseAtvBzz(uint8_t pin, bool cmd, String msg, unsigned int fAlrmFs, unsigned int fAlrmSl)
	: Morse(pin, msg, 0, 0, fAlrmFs, fAlrmSl),
	  buzzerCmd(cmd) {}

void MorseAtvBzz::setup()
{
	pinMode(buzzerPin, OUTPUT);
	digitalWrite(buzzerPin, !buzzerCmd);
}

void MorseAtvBzz::setQuiet()
{
	messageComplete = false;
	digitalWrite(buzzerPin, !buzzerCmd);
	currentCharIndex = 0;
	currentMarkIndex = 0;
	quiet = true;

	#if _MORSE_INTERRUPT

	#if defined(ARDUINO_ARCH_AVR)
	TIMSKn &= ~(1 << OCIEnA);
	#elif defined(ARDUINO_ARCH_ESP32)
	timerAlarmDisable(_morseTimer);
	#endif // defined(ARDUINO_ARCH_AVR) \ defined(ARDUINO_ARCH_AVR)

	#endif // _MORSE_INTERRUPT
}

bool MorseAtvBzz::playMorseChar(char c)
{

	volatile char const *seq = selectSeq(c);
	bool numAux = (c >= '0' && c<='9');
	long currentMillis = millis();

	if (currentMillis >= nextActionMillis)
	{
		// Serial.println((unsigned int)seq);
		if (currentMark == '\0')
		{
			currentMarkIndex = 0;
			currentMark = 0x16;
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			nextActionMillis = currentMillis + dotDelay;
			return 1;
		}
		if (currentMark == 0x16)
		{
			digitalWrite(buzzerPin, !buzzerCmd);
			currentMark = seq[currentMarkIndex++];
			nextActionMillis = currentMillis + dotDelay;
		}
		else if (currentMark == '.')
		{
			digitalWrite(buzzerPin, buzzerCmd);
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			currentMark = 0x16;
			nextActionMillis = currentMillis + dotDelay;
		}
		else if (currentMark == '-')
		{
			digitalWrite(buzzerPin, buzzerCmd);
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			currentMark = 0x16;
			nextActionMillis = currentMillis + dashDelay;
		}
		else if (currentMark == ' ')
		{
			digitalWrite(buzzerPin, !buzzerCmd);
			#if _MORSE_PRINT
			Serial.print(currentMark);
			#endif // _MORSE_PRINT
			currentMark = 0x16;
			nextActionMillis = currentMillis + dashDelay;
		}
		else if (currentMark == '~')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (-cos(M_PI * currentMarkIndex / 50.0) * 0.5 + 0.5);
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.tilde[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '^')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1- abs((2*currentMarkIndex - float(alarmSteps) - 1.0) / (float(alarmSteps) - 1.0)));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.hat[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}

			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '<')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (currentMarkIndex / float(alarmSteps));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.lt[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '#')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = ((currentMarkIndex / 50) % 2) ? ALARM_LOW : ALARM_HIGH;
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.num[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '>')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * ((alarmSteps - currentMarkIndex) / float(alarmSteps));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.gt[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '[')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.lsqb[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == ']')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.rsqb[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '{')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.lcub[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else if (currentMark == '}')
		{
			#if defined(ARDUINO_ARCH_AVR)
			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));
			#elif defined(ARDUINO_ARCH_ESP32)
			int signal = greatAlarmTable.rcub[currentMarkIndex % alarmSteps];
			#endif

			nextActionMillis = currentMillis + signal/2;

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
				#if _MORSE_PRINT
				Serial.print(currentMark);
				#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			alarmState = !alarmState;
			digitalWrite(buzzerPin, alarmState);
		}
		else
			currentMark = '\0';
	}
	return 0;
}