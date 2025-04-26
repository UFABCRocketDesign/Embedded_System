#include "Morse.h"

// For letters
static const char *const Morse::letters[] PROGMEM = {
	".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",	 // A-I
	".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", // J-R
	"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."		 // S-Z
};

// For Numbers
static const char *const Morse::numbers[] PROGMEM = {
	"-----", ".----", "..---", "...--", "....-",
	".....", "-....", "--...", "---..", "----."};

// For Space
static const char *const Morse::space[] PROGMEM = {"*"};

// For Morse
static const char *const Morse::morse[] PROGMEM = {"-", "."};

// For Alarms
static const char *const Morse::tilde[] PROGMEM = {"{", "|", "}", "~"};
static const char *const Morse::zigzag[] PROGMEM = {"[", "\\", "]", "^", "_"};
static const char *const Morse::compare[] PROGMEM = {"<", "=", ">"};

Morse::Morse(uint8_t pin, String msg = "", unsigned int fDot = 523, unsigned int fDash = 784, unsigned int fAlrmHi = 62, unsigned int fAlrmLo = 1976)
	: buzzerPin(pin),
	  currentMessage(msg),
	  FREQ_DOT(fDot),
	  FREQ_DASH(fDash),
	  ALARM_LOW(fAlrmHi),
	  ALARM_HIGH(fAlrmLo) {}

void Morse::setup()
{
	pinMode(buzzerPin, OUTPUT);
}

void Morse::setNextMessage(String message)
{
	nextMessage = message;
	newMessage = true;
}

void Morse::setQuiet()
{
	noTone(buzzerPin);
	currentCharIndex = 0;
	currentMarkIndex = 0;
	quiet = true;
}

bool Morse::getQuiet()
{
	return quiet;
}

bool Morse::playMorseChar(char c)
{
	char const *seq = 0;
	bool numAux = false;

	if (isDigit(c))
	{
		// seq = Morse::numbers[c - '0'];
		seq = pgm_read_ptr_near(Morse::numbers + (c - '0'));
		numAux = true;
	}
	else if (isUpperCase(c))
		seq = pgm_read_ptr_near(Morse::letters + (c - 'A'));
	else if (isLowerCase(c))
		seq = pgm_read_ptr_near(Morse::letters + (c - 'a'));
	else if (c == '-' || c == '.')
		seq = pgm_read_ptr_near(Morse::morse + (c - '-'));
	else if (c >= '{' && c <= '~')
		seq = pgm_read_ptr_near(Morse::tilde + (c - '{'));
	else if (c >= '[' && c <= '_')
		seq = pgm_read_ptr_near(Morse::zigzag + (c - '['));
	else if (c >= '<' && c <= '>')
		seq = pgm_read_ptr_near(Morse::compare + (c - '<'));
	else
		seq = pgm_read_ptr_near(Morse::space);

	float currentMillis = millis();

	if (currentMillis >= nextActionMillis)
	{
		// Serial.println((unsigned int)seq);
		if (currentMark == '\0')
		{
			currentMarkIndex = 0;
			currentMark = ' ';
#if _MORSE_PRINT
			Serial.print(currentMark);
#endif // _MORSE_PRINT
			nextActionMillis = currentMillis + dotDelay;
			return 1;
		}
		if (currentMark == ' ')
		{
			noTone(buzzerPin);
			currentMark = seq[currentMarkIndex++];
			nextActionMillis = currentMillis + dotDelay;
		}
		else if (currentMark == '.')
		{
			tone(buzzerPin, FREQ_DASH * (numAux ? 0.5 : 1));
#if _MORSE_PRINT
			Serial.print(currentMark);
#endif // _MORSE_PRINT
			currentMark = ' ';
			nextActionMillis = currentMillis + dotDelay;
		}
		else if (currentMark == '-')
		{
			tone(buzzerPin, FREQ_DOT * (numAux ? 0.5 : 1));
#if _MORSE_PRINT
			Serial.print(currentMark);
#endif // _MORSE_PRINT
			currentMark = ' ';
			nextActionMillis = currentMillis + dashDelay;
		}
		else if (currentMark == '*')
		{
			noTone(buzzerPin);
#if _MORSE_PRINT
			Serial.print(' ');
#endif // _MORSE_PRINT
			currentMark = ' ';
			nextActionMillis = currentMillis + dashDelay;
		}
		else if (currentMark == '~')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (-cos(M_PI * currentMarkIndex / 50.0) * 0.5 + 0.5);

			currentMarkIndex++;
			if (400 < currentMarkIndex)
			{
				currentMark = '\0';
#if _MORSE_PRINT
				Serial.print('~');
#endif // _MORSE_PRINT
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '^')
		{
			nextActionMillis = currentMillis + alarmDelay;
			if (ALARM_LOW > abs(currentMarkIndex))
			{
				if (abs(currentMarkIndex) > 1)
				{
#if _MORSE_PRINT
					Serial.print(currentMark);
#endif // _MORSE_PRINT
					currentMark = '\0';
				}
				currentMarkIndex = ALARM_LOW;
			}
			else if (ALARM_HIGH < abs(currentMarkIndex))
				currentMarkIndex = -ALARM_HIGH;
			currentMarkIndex += 10;

			tone(buzzerPin, abs(currentMarkIndex));
		}
		else if (currentMark == '<')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (currentMarkIndex / 400.0);

			currentMarkIndex++;
			if (400 < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '=')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = ((currentMarkIndex / 50) % 2) ? ALARM_LOW : ALARM_HIGH;

			currentMarkIndex++;
			if (400 < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '>')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * ((400 - currentMarkIndex) / 400.0);

			currentMarkIndex++;
			if (400 < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '[')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == ']')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '{')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow(1.0 - (currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '}')
		{
			nextActionMillis = currentMillis + alarmDelay;

			int signal = long(ALARM_LOW) + long(ALARM_HIGH - ALARM_LOW) * (1.0 - pow((currentMarkIndex - 1.0) / (float(alarmSteps) - 1.0), 8));

			currentMarkIndex++;
			if (alarmSteps < currentMarkIndex)
			{
#if _MORSE_PRINT
				Serial.print(currentMark);
#endif // _MORSE_PRINT
				currentMark = '\0';
			}
			tone(buzzerPin, signal);
		}
		else if (currentMark == '*')
		{
			noTone(buzzerPin);
			currentMark = ' ';
#if _MORSE_PRINT
			Serial.print(currentMark);
#endif // _MORSE_PRINT
			nextActionMillis = currentMillis + dashDelay;
		}
		else
			currentMark = '\0';
	}
	return 0;
}

bool Morse::updateMorse()
{
	quiet = false;
	if (currentCharIndex == 0)
	{
		if (newMessage)
		{
			currentMessage = nextMessage;
			newMessage = false;
		}
	}
	if (currentCharIndex <= currentMessage.length())
	{
		byte ret = playMorseChar(currentMessage[currentCharIndex]);
		if (ret)
			currentCharIndex++;
	}
	else
	{
		currentCharIndex = 0;
#if _MORSE_PRINT
		Serial.println();
#endif // _MORSE_PRINT
		return true;
	}
	return false;
}
