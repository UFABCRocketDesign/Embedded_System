/////////////////////////////////////////////////CHOOSE BOARD/////////////////////////////////////////////////

#pragma region ShieldSelection

#include "src/lib/boards.h"

#if defined(ARDUINO_ARCH_AVR)
	#define USING_BOARD MEGA_OCTA_PTH_MK_I
	// #define USING_BOARD MEGA_STACK_DADOS_ACIONAMENTO_2019
	// #define USING_BOARD MEGA_STACK_DADOS_ACIONAMENTO_2020
#elif defined(ARDUINO_ARCH_ESP32)
	// #define USING_BOARD ESP_ESSENTIALS_2025
	// #define USING_BOARD ESP_ESSENTIALS_2026
	#define USING_BOARD ESP_MAIN_SMD_2026
	// #define USING_BOARD ESP_JOHN_SI_SMD_2026
#else
	#error Nao existem placas para a arquitetura selecionada
#endif

#include "src/lib/pinos.h"

#pragma endregion

//////////////////////////////////////////////////CHOOSE MODE//////////////////////////////////////////////////

#pragma region ModeSelection

#include "src/lib/modes.h"

// #define USING_MODE MODE_LANCAMENTO_1K
#define USING_MODE MODE_LANCAMENTO_3K
// #define USING_MODE MODE_LANCAMENTO_10K
// #define USING_MODE MODE_ELEVADOR
// #define USING_MODE MODE_ASPIRADOR
// #define USING_MODE MODE_MANUAL
// #define USING_MODE MODE_VIRTUAL

#include "src/lib/presets.h"

#pragma endregion

/////////////////////////////////////////////////CONFIGURATION/////////////////////////////////////////////////

#pragma region Configurations

#include "src/lib/configurations.hpp"

#pragma endregion

/////////////////////////////////////////////////////objects///////////////////////////////////////////////////

#pragma region Declarations

#include "src/lib/declarations.hpp"

#pragma endregion

/////////////////////////////////////////////////////SETUP/////////////////////////////////////////////////////

#pragma region Setup
void setup()
{
#if defined(ARDUINO_ARCH_ESP32) && (USE_APOGEE_DETECTION || USE_LoRa_E32_settable)
	EEPROM.begin(_EEPROM_SIZE);
#endif // defined(ARDUINO_ARCH_ESP32) && (USE_APOGEE_DETECTION || USE_LoRa_E32_settable)
#if USE_APOGEE_LED
	pinMode(PWMout, OUTPUT);
#endif // USE_APOGEE_LED

#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT
	pinMode(RBF_PIN, INPUT_PULLUP);
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT

#if USE_RGB
	pinMode(RGB_rPin, OUTPUT);
	pinMode(RGB_gPin, OUTPUT);
	pinMode(RGB_bPin, OUTPUT);
	digitalWrite(RGB_rPin, !RGB_CMD);
	digitalWrite(RGB_gPin, !RGB_CMD);
	digitalWrite(RGB_bPin, !RGB_CMD);
#endif // USE_RGB

	beginEverything();			// Begin every module and subsystem

#if USE_STORAGE
	printHeaderSD();			// Build header in file
#endif // USE_STORAGE

#if HAS_COMM_CHANNEL
	transmitln(F(" "));
#endif // HAS_COMM_CHANNEL

#if USE_VIRTUAL
	if(Bridge.count() >= VB_MAX_SENSORS)
	{
		transmitln(F("\nERRO: Tabela de ponte sobrecarregada!"));
	}
#endif // USE_VIRTUAL

	////////////////USE_SOFTWARE_REMOVE_BEFORE_FLIGHT directive////////////////

#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT
	RemoveBefore();
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT

	////////////////USE_SOFTWARE_REMOVE_BEFORE_FLIGHT directive////////////////

#if PRINT_ANY
	printHeaderSerial();		// Show header in Serial
#endif // PRINT_ANY

#if USE_STORAGE
	SDC.util.begin();
#endif // USE_STORAGE

	////////////////USE_WAIT_UNTIL_FLIGHT directive////////////////

#if USE_WAIT_UNTIL_FLIGHT
	WaitUntilFlight(CURRENT_MODE_WUFheigh);
#if HAS_COMM_CHANNEL
	transmit(F("LiftOff confirmed"));
#endif // HAS_COMM_CHANNEL
#endif // USE_WAIT_UNTIL_FLIGHT

	////////////////USE_WAIT_UNTIL_FLIGHT directive////////////////

	Gutil.begin();

#if USE_LoRa
	LRutil.begin();
#endif // USE_LoRa

#if USE_APOGEE_DETECTION
	apg.resetTimer();
#endif // USE_APOGEE_DETECTION
#if USE_DEPLOY
	rec.resetTimer();
#endif // USE_DEPLOY
#if USE_APOGEE_DETECTION && USE_WAIT_UNTIL_FLIGHT
	apg.markLiftoff();	// persiste 'ja decolou neste build' (apos os timers)
#endif // USE_APOGEE_DETECTION && USE_WAIT_UNTIL_FLIGHT

#if USE_WAIT_PRESSURE_STABLE
	WaitUntilPressureStabilize(CURRENT_MODE_WUPSdelay);
#if HAS_COMM_CHANNEL
	transmit(F("Pressure Stabilize timer expired"));
#endif // HAS_COMM_CHANNEL
#endif // USE_WAIT_PRESSURE_STABLE

}
#pragma endregion

/////////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////

#pragma region Loop
void loop()
{
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
	sysC = 0;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT

	Gutil.counter();
	readEverything();

#if USE_APOGEE_DETECTION
	if(baroHasData) apg.calcHeight(baro.getPressure());
#if USE_DEPLOY
	rec.emergency(baro.getTimeLapse() > 1000000 * LapsMaxT);
	if (rec.getGlobalState())
	{
		apg.apgSigma();
		apg.apgAlpha();
		rec.sealApogee(apg.getApogeu(0.8f));
		rec.putHeight(apg.getHeight());
		rec.refresh();
#if PRINT_APOGEE_SIGMA_MAX
		Gutil.comparer(apg.getSigma());
#endif // PRINT_APOGEE_SIGMA_MAX
		if (rec.getApogee()) Gutil.mem = 1;
	}
	else {
#if USE_MORSE_MSG
		if(Mutil.oneTime())
		{
			mensageiro.msgAux = F("| ~ | # | ^ | < | > | [ | ] | { | } | _ | $ | % A ");
			mensageiro.msgAux += String(apg.getApgPt());
			mensageiro.msgAux += F(" m");
			mensageiro.setNextMessage(mensageiro.msgAux);
			mensageiro.msgAux = "";
			#if MORSE_INTERRUPT
			mensageiro.unsetQuiet();
			#endif // MORSE_INTERRUPT
		}
		#if !MORSE_INTERRUPT
		mensageiro.updateMorse();
		#endif // !MORSE_INTERRUPT
#endif // USE_MORSE_MSG
#if USE_BEEP_SYSTEM_COUNT
		beep(SYSTEM_COUNT); //rec
#endif // USE_BEEP_SYSTEM_COUNT
	}
#endif // USE_DEPLOY
#endif // USE_APOGEE_DETECTION
#if USE_APOGEE_LED
	analogWrite(PWMout, (char)(apg.getSigma() * 255));
#endif // USE_APOGEE_LED

#if ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))
	SerialSend();
#endif // ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))

#if USE_STORAGE
	SDSend();
#endif // USE_STORAGE

#if USE_LoRa
	LoRaSend();
#endif // USE_LoRa

#if USE_MORSE_MSG && FORCE_SYSTEM_COUNT && (!MORSE_INTERRUPT)
	mensageiro.updateMorse();
#endif // USE_MORSE_MSG  && FORCE_SYSTEM_COUNT && (!MORSE_INTERRUPT)
#if USE_BEEP_SYSTEM_COUNT && FORCE_SYSTEM_COUNT
	beep(sysC);
#elif USE_BEEP_SYSTEM_COUNT && !USE_WAIT_UNTIL_FLIGHT && !USE_SOFTWARE_REMOVE_BEFORE_FLIGHT && !USE_APOGEE_DETECTION
	beep(1);
#endif // USE_BEEP_SYSTEM_COUNT && !USE_WAIT_UNTIL_FLIGHT && !USE_SOFTWARE_REMOVE_BEFORE_FLIGHT && !USE_APOGEE_DETECTION
}
#pragma endregion

#pragma region Modules


//////////////////////////////////////////////////////BET//////////////////////////////////////////////////////

inline void beginEverything()
{

#if USE_DEPLOY
	rec.begin();
#endif // USE_DEPLOY

#if USE_DUAL_DEPLOY
	rec.mainN.setHeightCmd(CURRENT_MODE_P2H_NORMAL);
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_DELAYED
	rec.mainN.setDelayCmd(sysDelay_main);
#endif // USE_MAIN_DELAYED

// Main
#if USE_MAIN_BACKUP
#if USE_DUAL_DEPLOY
	rec.mainB.setHeightCmd(CURRENT_MODE_P2H_BACKUP);
#else
#if USE_MAIN_DELAYED
	rec.mainB.setDelayCmd(sysDelay + sysDelay_main);
#else
	rec.mainB.setDelayCmd(sysDelay);
#endif // USE_MAIN_DELAYED
#endif // !USE_DUAL_DEPLOY
#endif // USE_MAIN_BACKUP

// Drogue
#if USE_DROGUE_BACKUP
	rec.drogB.setDelayCmd(sysDelay);
#endif // USE_DROGUE_BACKUP

#if ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING) || (USE_VIRTUAL))
	Serial.begin(SERIAL_BAUD_RATE);

	#if defined(ARDUINO_ARCH_ESP32)
	delay(ESP32_WAIT_TIME_MS); // Wait to ESP32 properly start
	#endif // defined(ARDUINO_ARCH_ESP32)

	Serial.println();
#if PRINT_SYSTEM_COUNT
	Serial.print(F("System number: "));
	Serial.println(SYSTEM_COUNT);
#endif // PRINT_SYSTEM_COUNT
#endif // ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))


#if USE_BUZZER_ACTIVE
	pinMode(BUZZER_PIN, OUTPUT);
	digitalWrite(BUZZER_PIN, !BUZZER_ON_LEVEL);
#endif // USE_BUZZER_ACTIVE
#if USE_MORSE_MSG
	mensageiro.setup();

#if MORSE_INTERRUPT
	morseInterruptionSetup();
#endif // MORSE_INTERRUPT

	mensageiro.updateMorse();

#endif // USE_MORSE_MSG
#if USE_BEEP_SYSTEM_COUNT
	beep();
#endif // USE_BEEP_SYSTEM_COUNT

#if USE_LoRa

#ifdef ARDUINO_ARCH_ESP32
	LoRa.begin(LoRaBaudRate, SERIAL_8N1, RX_LORA_ESP, TX_LORA_ESP);
#else
	LoRa.begin(LoRaBaudRate);
#endif // ARDUINO_ARCH_ESP32

#if USE_LoRa_E32
#if USE_LoRa_E32_settable
	configLoRa.begin();
#else
	pinMode(M0_LORA_PIN,OUTPUT); digitalWrite(M0_LORA_PIN,LOW);
	pinMode(M1_LORA_PIN,OUTPUT); digitalWrite(M1_LORA_PIN,LOW);
#endif  // USE_LoRa_E32_settable
#endif // USE_LoRa_E32

#endif // USE_LoRa

#if REPORT_RESET_REASON && defined(ARDUINO_ARCH_ESP32)
	transmit(F("\nReset: "));
	transmit(resetReasonName(esp_reset_reason()));
#endif // REPORT_RESET_REASON && defined(ARDUINO_ARCH_ESP32)

#if USE_GNSS
	GpS.begin();
	GpS.util.mem = false; // Auxiliar de primeira leitura
	GpS.util.forT(60);
	if (GpS)
	{
		if (GpS.util.oneTime())
		{
#if HAS_COMM_CHANNEL
			transmit(F("\nGPS ok "));
			transmit(GpS.getLatitude(), 6);
			transmit(F(", "));
			transmit(GpS.getLongitude(), 6);
#endif // HAS_COMM_CHANNEL
		}
	}
	else
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nGPS err, waiting signal"));
#endif // HAS_COMM_CHANNEL
	}
#endif // USE_GNSS

#if HAS_I2C_SENSOR
#if ARDUINO_ARCH_ESP32
	Wire.begin(SDA_I2C_ESP, SCL_I2C_ESP);
	Wire.setClock(400000);
	Wire.setTimeOut(3000);
#else
	Wire.begin();
#endif // ARDUINO_ARCH_ESP32
#endif // HAS_I2C_SENSOR

#if USE_BARO
	baro.begin();
	if (baro)
	{
#if (!USE_BMP085)
		for (short i = 0; i < 50; i++) baro.readAll(); // Contornar tempo de estabilização do filtro interno
#endif // (!USE_BMP085)
#if USE_APOGEE_DETECTION
		for (short i = 0; i < 100; i++) if (baro) apg.addZero(baro.getPressure());
		apg.fixZero(CURRENT_MODE_Fix0range, CURRENT_MODE_Fix0diff);
#endif // USE_APOGEE_DETECTION
#if HAS_COMM_CHANNEL
		transmit(F("\nBaro ok "));
#endif // HAS_COMM_CHANNEL

#if PRINT_GROUND_REFERENCE
		if(apg.getFixZero()){
			Serial.print(F("(Using EEPROM Zero Ref @ < 0x"));
			Serial.print(apg.getEEAddress(), HEX);
			Serial.print(F(" >!) "));
		}

		Serial.print(apg.getZero());
		Serial.println();
#elif USE_PRINT
		Serial.println();
#endif // USE_PRINT

#if USE_LoRa
#if USE_APOGEE_DETECTION
		// When system gets a invalid zero height (reset on flight)
		if(apg.getFixZero()) {

			#if USE_LoRa_E32_settable
			LoRa.print(F("(Using EEPROM LoRa settings: ADD < 0x"));
			LoRa.print(configLoRa.getCurrentConfigurations().ADDH, HEX);
			LoRa.print(configLoRa.getCurrentConfigurations().ADDL, HEX);
			LoRa.print(F(" > CHAN < 0x"));
			LoRa.print(configLoRa.getCurrentConfigurations().CHAN, HEX);
			LoRa.print(F(" > Ref @ < 0x "));
			LoRa.print(configLoRa.getEEAddress(), HEX);
			LoRa.print(F(" >)! "));

			if(configLoRa.getEEValid())
			{
				LoRa.println(F("Success!"));
			}
			else
			{
				LoRa.println(F("Fail, using default LoRa settings"));
			}
			#endif // USE_LoRa_E32_settable

			LoRa.print(F("(Using EEPROM Zero Ref @ < 0x"));
			LoRa.print(apg.getEEAddress(), HEX);
			LoRa.print(F(" >!) "));
		}
		LoRa.println(apg.getZero());
#endif // USE_APOGEE_DETECTION
		LoRa.println();
#endif // USE_LoRa

	}
	else
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nBaro err"));
#endif // HAS_COMM_CHANNEL
	}
#endif // USE_BARO

#if USE_ACCEL
	accel.begin();
	if (accel)
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nAccel ok"));
#endif // HAS_COMM_CHANNEL
	}
	else
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nAccel err"));
#endif // HAS_COMM_CHANNEL
	}
#endif // USE_ACCEL

#if USE_GYRO
	giro.begin();
	if (giro)
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nGiro ok"));
#endif // HAS_COMM_CHANNEL
	}
	else
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nGiro err"));
#endif // HAS_COMM_CHANNEL
	}
#endif // USE_GYRO

#if USE_MAGN
	magn.begin();
	if (magn)
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nMagn ok"));
#endif // HAS_COMM_CHANNEL
	}
	else
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nMagn err"));
#endif // HAS_COMM_CHANNEL
	}
#endif // USE_MAGN

#if USE_DEPLOY && HAS_COMM_CHANNEL
	transmit(rec.mainN.info() ? F("\nIgnMainN ok") : F("\nIgnMainN err"));

#if USE_DUAL_DEPLOY
	transmit(rec.drogN.info() ? F("\nIgnDrogN ok") : F("\nIgnDrogN err"));
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
	transmit(rec.mainB.info() ? F("\nIgnMainB ok") : F("\nIgnMainB err"));
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
	transmit(rec.drogB.info() ? F("\nIgnDrogB ok") : F("\nIgnDrogB err"));
#endif // USE_DROGUE_BACKUP


#endif // USE_DEPLOY && HAS_COMM_CHANNEL
}

////////////////////////////////////////////USE_SOFTWARE_REMOVE_BEFORE_FLIGHT///////////////////////////////////////////

#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT
inline void RemoveBefore()
{
	bool rbf = 0;
	Helpful rbfHelper;
	do
	{
		sysC = 0;
		readEverything();
		rbf = digitalRead(RBF_PIN);
		if (rbfHelper.oneTime())
		{
#if USE_PRINT
			Serial.print(sysC);
			Serial.print(F(" parts of "));
			Serial.print(SYSTEM_COUNT);
			Serial.print(F(" working, waiting...\t"));
#if USE_BARO
			Serial.print(baro.getTemperature());
			Serial.print(F(" "));
			Serial.write(0xB0);
#endif // USE_BARO
#if USE_GNSS
			Serial.print(F("C\tLat: "));
			Serial.print(GpS.getLatitude(), 6);
			Serial.print(F("\tLon: "));
			Serial.print(GpS.getLongitude(), 6);
			Serial.print(F("\t"));
			Serial.print(GpS.getDay());
			Serial.print('/');
			Serial.print(GpS.getMonth());
			Serial.print('\t');
			Serial.print(GpS.getHour());
			Serial.print(':');
			Serial.print(GpS.getMinute());
			Serial.print(':');
			Serial.print(GpS.getSecond());
#endif // USE_GNSS

			Serial.println();

#endif // USE_PRINT
		}
		else if (rbfHelper.eachT(2)) rbfHelper.oneTimeReset();
#if USE_LoRa
#if USE_LoRa_E32_settable
		configLoRa.updateFrequency();
#endif // USE_LoRa_E32_settable
		LoRaSend();
#endif // USE_LoRa

#if USE_BEEP_SYSTEM_COUNT
		///////////////////////////////////////

		beep(sysC);

		///////////////////////////////////////

#endif // USE_BEEP_SYSTEM_COUNT

	} while (!rbf);

#if USE_LoRa_E32_settable
	configLoRa.discardPauseTelemetry(); // Garante telemetria ativa ao entrar em voo
#endif // USE_LoRa_E32_settable

#if USE_BEEP_SYSTEM_COUNT
	beep();
#endif // USE_BEEP_SYSTEM_COUNT
}
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT

//////////////////////////////////////////////////////USE_WAIT_UNTIL_ROUTINES//////////////////////////////////////////////////////

#if USE_WAIT_UNTIL_ROUTINES
inline void WaitUntil()
{
	sysC = 0;
	#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux = "";
	#endif // USE_MORSE_MSG
			Gutil.counter();
			readEverything();

	#if USE_APOGEE_DETECTION
			if(baroHasData) apg.calcHeight(baro.getPressure());
			apg.apgSigma();
			apg.apgAlpha();
	#if PRINT_APOGEE_SIGMA_MAX
			Gutil.comparer(apg.getSigma());
	#endif // PRINT_APOGEE_SIGMA_MAX
	#endif // USE_APOGEE_DETECTION
	#if USE_APOGEE_LED
			analogWrite(PWMout, (char)(apg.getSigma() * 255));
	#endif // USE_APOGEE_LED

	#if ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))
			SerialSend();
	#endif // ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))

	#if USE_STORAGE
			SDSend();
	#endif // USE_STORAGE

	#if USE_LoRa
			LoRaSend();
	#endif // USE_LoRa
}
#endif // USE_WAIT_UNTIL_ROUTINES

#if USE_WAIT_UNTIL_FLIGHT
inline void WaitUntilFlight(float minHeight)
{
	do
	{
		#if USE_LoRa_E32_settable
		configLoRa.updateFrequency();
		#endif // USE_LoRa_E32_settable

		WaitUntil();

#if USE_MORSE_MSG
		// if(Mutil.oneTime()) if(mensageiro.msgAux.length() > 0) mensageiro.setNextMessage("# "+mensageiro.msgAux);
		// if(mensageiro.updateMorse()) Mutil.oneTimeReset();

		if(mensageiro.msgAux.length() > 0) { // Any system failed
			if(Mutil.oneTime()) mensageiro.setNextMessage("# "+mensageiro.msgAux);
			#if MORSE_INTERRUPT
			mensageiro.unsetQuiet();
			#else
			mensageiro.updateMorse();
			#endif // !MORSE_INTERRUPT
			if(mensageiro.getMessageComplete()) Mutil.oneTimeReset();
		} else { // No system failed
			if(!mensageiro.getQuiet())
			{
				#if !MORSE_INTERRUPT
				mensageiro.updateMorse();
				#endif // !MORSE_INTERRUPT
				if(mensageiro.getMessageComplete()) // After sound sequence completed
				{
					mensageiro.setQuiet(); // Mute sound
					Mutil.forT(ALARM_DELAY); // Wait some time
				}
			}
			else {
				if(!Mutil.forT()) {
					#if MORSE_INTERRUPT
					mensageiro.unsetQuiet();
					#else
					mensageiro.updateMorse(); // Restart sound
					#endif // MORSE_INTERRUPT
				}
			}
		}
#endif // USE_MORSE_MSG
#if USE_BEEP_SYSTEM_COUNT
		beep(sysC);
#endif // USE_BEEP_SYSTEM_COUNT

	}
	while (abs(apg.getHeight()) < minHeight && !(baro.getTimeLapse() > 1000000 * LapsMaxT));

#if USE_LoRa_E32_settable
	configLoRa.discardPauseTelemetry(); // Garante telemetria ativa ao entrar em voo
#endif // USE_LoRa_E32_settable

#if USE_MORSE_MSG
	mensageiro.setQuiet();
	Mutil.oneTimeReset();
#endif // USE_MORSE_MSG
#if USE_BEEP_SYSTEM_COUNT
	beep();
#endif // USE_BEEP_SYSTEM_COUNT
}
#endif // USE_WAIT_UNTIL_FLIGHT

#if USE_WAIT_PRESSURE_STABLE
inline void WaitUntilPressureStabilize(float waitTime) {
	Gutil.forT(waitTime);
	while (Gutil.forT())
	{
		WaitUntil();
	}
	// Ensure that apogee point was not given during pressure anomaly
	apg.resetAptPt();
}
#endif // USE_WAIT_PRESSURE_STABLE

//////////////////////////////////////////////////////SPT//////////////////////////////////////////////////////

#if ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))
inline void SerialSend()
{

#if PRINT_LOOP_RATE
	if (Gutil.eachN(100))
	{
		Serial.print(Gutil.getCount());
		Serial.print('\t');
		float T = Gutil.lapse();
		Serial.print(T, 5);
		Serial.print('\t');
		Serial.println(1 / T, 5);
	}
	else
	{
		Gutil.lapse();
	}
#endif // PRINT_LOOP_RATE

#if PRINT_LOOP_COUNT
	Serial.print(Gutil.getCount());
	Serial.print(F(":\t"));
#endif // PRINT_LOOP_COUNT

#if PRINT_PERFORMANCE_TIMING // Print elapsed iteration time every 100 iterations, for performance tests only
	static Helpful G;
	static float a;

	float b= Gutil.sinceBegin();

	//Serial.print((b-a)*500, 3);
	if ((b - a) * 100 > 1) Serial.println(G.lapse(),6);
	else G.lapse();
	a = b;
#endif // PRINT_PERFORMANCE_TIMING

#if PRINT_TIME
	Serial.print(Gutil.sinceBegin());
	Serial.print('\t');
#endif // PRINT_TIME

	////////////////////////////////////////////////////
#if PRINT_SEPARATORS && (PRINT_ACCEL_X || PRINT_ACCEL_Y || PRINT_ACCEL_Z)
	Serial.print('|');
#endif // PRINT_SEPARATORS && (PRINT_ACCEL_X || PRINT_ACCEL_Y || PRINT_ACCEL_Z)
#if PRINT_ACCEL_X
	Serial.print(MM_accel[0], 3);
	Serial.print('\t');
#endif // PRINT_ACCEL_X

#if PRINT_ACCEL_Y
	Serial.print(MM_accel[1], 3);
	Serial.print('\t');
#endif // PRINT_ACCEL_Y

#if PRINT_ACCEL_Z
	Serial.print(MM_accel[2], 3);
	Serial.print('\t');
#endif // PRINT_ACCEL_Z

	/////////////////////////////////////////////////////
#if PRINT_SEPARATORS && (PRINT_GYRO_X || PRINT_GYRO_Y || PRINT_GYRO_Z)
	Serial.print('|');
#endif // PRINT_SEPARATORS && (PRINT_GYRO_X || PRINT_GYRO_Y || PRINT_GYRO_Z)
#if PRINT_GYRO_X
	Serial.print(MM_giro[0], 1);
	Serial.print('\t');
#endif // PRINT_GYRO_X

#if PRINT_GYRO_Y
	Serial.print(MM_giro[1], 1);
	Serial.print('\t');
#endif // PRINT_GYRO_Y

#if PRINT_GYRO_Z
	Serial.print(MM_giro[2], 1);
	Serial.print('\t');
#endif // PRINT_GYRO_Z

	////////////////////////////////////////////////////
#if PRINT_SEPARATORS && (PRINT_MAGN_X || PRINT_MAGN_Y || PRINT_MAGN_Z)
	Serial.print('|');
#endif // PRINT_SEPARATORS && (PRINT_MAGN_X || PRINT_MAGN_Y || PRINT_MAGN_Z)
#if PRINT_MAGN_X
	Serial.print(MM_magn[0], 1);
	Serial.print('\t');
#endif // PRINT_MAGN_X
#if PRINT_MAGN_Y
	Serial.print(MM_magn[1], 1);
	Serial.print('\t');
#endif // PRINT_MAGN_Y
#if PRINT_MAGN_Z
	Serial.print(MM_magn[2], 1);
	Serial.print('\t');
#endif // PRINT_MAGN_Z

	////////////////////////////////////////////////////
#if PRINT_SEPARATORS && (PRINT_BARO_TEMPERATURE || PRINT_BARO_PRESSURE)
	Serial.print('|');
#endif // PRINT_SEPARATORS && (PRINT_BARO_TEMPERATURE || PRINT_BARO_PRESSURE)
#if PRINT_BARO_TEMPERATURE
	Serial.print(MM_baro[0], 1);
	Serial.print('\t');
#endif // PRINT_BARO_TEMPERATURE
#if PRINT_BARO_PRESSURE
	Serial.print(MM_baro[1], 1);
	Serial.print('\t');
#endif // PRINT_BARO_PRESSURE

	/////////////////////////////////////////////////////
#if PRINT_SEPARATORS && PRINT_HEIGHT
	Serial.print('|');
#endif // PRINT_SEPARATORS && PRINT_HEIGHT
#if PRINT_HEIGHT
	Serial.print(apg.getHeight());
	Serial.print('\t');
#endif // PRINT_HEIGHT

#if PRINT_SEPARATORS && PRINT_PEAK_HEIGHT
	Serial.print('|');
#endif // PRINT_SEPARATORS && PRINT_PEAK_HEIGHT
#if PRINT_PEAK_HEIGHT
	Serial.print(apg.getApgPt());
	Serial.print('\t');
	Serial.print(apg.getApgTm());
	Serial.print('\t');
#endif // PRINT_PEAK_HEIGHT

#if PRINT_SEPARATORS && (PRINT_APOGEE_ALPHA || PRINT_APOGEE_SIGMA)
	Serial.print('|');
#endif // PRINT_SEPARATORS
#if PRINT_APOGEE_ALPHA
	Serial.print(apg.getAlpha());
	Serial.print('\t');
#endif // PRINT_APOGEE_ALPHA
#if PRINT_APOGEE_SIGMA
	Serial.print(apg.getSigma(), 7);
	Serial.print('\t');
#endif // PRINT_APOGEE_SIGMA


	/////////////////////////////////////////////////////
#if PRINT_SEPARATORS && PRINT_APOGEE_SIGMA_MAX
	Serial.print('|');
#endif // PRINT_SEPARATORS && PRINT_APOGEE_SIGMA_MAX
#if PRINT_APOGEE_SIGMA_MAX
	Serial.print(Gutil.getMax(), 5);
	Serial.print('\t');
#endif // PRINT_APOGEE_SIGMA_MAX



#if PRINT_GNSS
#if PRINT_SEPARATORS
	Serial.print('|');
#endif // PRINT_SEPARATORS
	Serial.print(GpS.getLatitude(), 6);
	Serial.print('\t');
	Serial.print(GpS.getLongitude(), 6);
	Serial.print('\t');
	Serial.print(GpS.getAltitude());
	Serial.print('\t');
	Serial.print(GpS.getMps(), 3);
	Serial.print('\t');
	Serial.print(GpS.getSatellites());
	Serial.print('\t');
	Serial.print(GpS.getPrecision());
	Serial.print('\t');
	Serial.print(GpS.getChars());
	Serial.print('\t');
	Serial.print(GpS.getYear());
	Serial.print('-');
	Serial.print(GpS.getMonth());
	Serial.print('-');
	Serial.print(GpS.getDay());
	Serial.print('\t');
	Serial.print(GpS.getHour());
	Serial.print(':');
	Serial.print(GpS.getMinute());
	Serial.print(':');
	Serial.print(GpS.getSecond());
	Serial.print('\t');
	Serial.print(GpS.getKph());
	Serial.print('\t');
#endif // PRINT_GNSS

#if PRINT_RECOVERY_HEALTH
#if PRINT_SEPARATORS
	Seria.print('|');
#endif // PRINT_SEPARATORS
	Serial.print(rec.mainN.info());
	Serial.print('\t');
#if USE_DUAL_DEPLOY
	Serial.print(rec.drogN.info());
	Serial.print('\t');
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_BACKUP
	Serial.print(rec.mainB.info());
	Serial.print('\t');
#endif // USE_MAIN_BACKUP
#if USE_DROGUE_BACKUP
	Serial.print(rec.drogB.info());
	Serial.print('\t');
#endif // USE_DROGUE_BACKUP
#endif // PRINT_RECOVERY_HEALTH

#if PRINT_APOGEE_EVENT
	if (Gutil.mem)
	{
		Serial.print(F("H: "));
		Serial.print(apg.getApgPt());
		Serial.print(F(" m\tT:"));
		Serial.print(apg.getApgTm());
		Serial.print(F(" s\t"));
	}
#if HAS_COMM_CHANNEL
#if USE_DEPLOY
	if (rec.getGlobalState())
	{
		if (rec.mainN.getState(0))
		{
#if USE_STORAGE
			rec.mainN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act MainN:"));
			Serial.print(rec.mainN.getDeploymentHeight());
			Serial.print(F("m\t"));
		}

#if USE_DUAL_DEPLOY
		if (rec.drogN.getState(0))
		{
#if USE_STORAGE
			rec.drogN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act DrogueN:"));
			Serial.print(rec.drogN.getDeploymentHeight());
			Serial.print(F("m\t"));
		}
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
		if (rec.mainB.getState(0))
		{
#if USE_STORAGE
			rec.mainB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act MainB:"));
			Serial.print(rec.mainB.getDeploymentHeight());
			Serial.print(F("m\t"));
		}
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
		if (rec.drogB.getState(0))
		{
#if USE_STORAGE
			rec.drogB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_STORAGE
			Serial.print(F("Act DrogueB:"));
			Serial.print(rec.drogB.getDeploymentHeight());
			Serial.print(F("m\t"));
		}

#endif // USE_DROGUE_BACKUP
	}
#endif // USE_DEPLOY
#endif // HAS_COMM_CHANNEL

#endif // PRINT_APOGEE_EVENT

#if PRINT_ANY
	Serial.println();
#endif // PRINT_ANY

}
#endif // ((USE_PRINT) || (PRINT_PERFORMANCE_TIMING))

//////////////////////////////////////////////////////SDC//////////////////////////////////////////////////////

#if USE_STORAGE
inline void SDSend()
{
	if (!SDC.util.mem)
	{
		if (SDC)
		{
			SDC.theFile.print(SDC.util.sinceBegin(), 3); SDC.tab();
#if USE_ACCEL
			for (short i = 0; i < 3; i++) { SDC.theFile.print(MM_accel[i], 3);	SDC.tab(); }
#endif // USE_ACCEL
#if USE_GYRO
			for (short i = 0; i < 3; i++) { SDC.theFile.print(MM_giro[i], 1);	SDC.tab(); }
#endif // USE_GYRO
#if USE_MAGN
			for (short i = 0; i < 3; i++) { SDC.theFile.print(MM_magn[i], 1);	SDC.tab(); }
#endif // USE_MAGN
#if USE_BARO
			for (short i = 0; i < 2; i++) { SDC.theFile.print(MM_baro[i]);		SDC.tab(); }
#if USE_APOGEE_DETECTION
			SDC.theFile.print(apg.getHeight()); SDC.tab();
#endif // USE_APOGEE_DETECTION

#endif // USE_BARO

#if LOG_GPS
#if (!LOG_GPS_EVERY_LINE)
			if (GpS.isNew())
			{
#endif // (!LOG_GPS_EVERY_LINE)
				SDC.theFile.print(GpS.getLatitude(), 6);	SDC.tab(); //Latitude
				SDC.theFile.print(GpS.getLongitude(), 6);	SDC.tab(); //Longitude
				SDC.theFile.print(GpS.getAltitude());		SDC.tab(); //Altitude
				SDC.theFile.print(GpS.getMps());			SDC.tab(); //Velocidade
				SDC.theFile.print(GpS.getSatellites());		SDC.tab(); //Numero de satelites
				SDC.theFile.print(GpS.getPrecision());		SDC.tab(); //Precisao
#if (!LOG_GPS_EVERY_LINE)
			}
			else
			{
				SDC.tab();
				SDC.tab();
				SDC.tab();
				SDC.tab();
				SDC.tab();
				SDC.tab();
			}
#endif // (!LOG_GPS_EVERY_LINE)
			SDC.theFile.print(GpS.isNew());				SDC.tab(); //Fix novo nesta linha
#endif // LOG_GPS

#if LOG_APOGEE
			SDC.theFile.print(apg.getAlpha());				SDC.tab(); //Apogeu declarado
			SDC.theFile.print(apg.getSigma(), 3);			SDC.tab(); //Confianca do apogeu
#endif // LOG_APOGEE
#if LOG_DEPLOY_EVERY_LINE
			SDC.theFile.print(rec.mainN.getGlobalState(0));	SDC.tab(); //Main normal ja acionou
			SDC.theFile.print(rec.mainN.getState(1));		SDC.tab(); //Main normal disparando agora
#if USE_DUAL_DEPLOY
			SDC.theFile.print(rec.drogN.getGlobalState(0));	SDC.tab();
			SDC.theFile.print(rec.drogN.getState(1));		SDC.tab();
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_BACKUP
			SDC.theFile.print(rec.mainB.getGlobalState(0));	SDC.tab();
			SDC.theFile.print(rec.mainB.getState(1));		SDC.tab();
#endif // USE_MAIN_BACKUP
#if USE_DROGUE_BACKUP
			SDC.theFile.print(rec.drogB.getGlobalState(0));	SDC.tab();
			SDC.theFile.print(rec.drogB.getState(1));		SDC.tab();
#endif // USE_DROGUE_BACKUP
#endif // LOG_DEPLOY_EVERY_LINE

#if LOG_IGNITER_HEALTH
			SDC.theFile.print(rec.mainN.info());			SDC.tab(); //Continuidade main normal
#if USE_DUAL_DEPLOY
			SDC.theFile.print(rec.drogN.info());			SDC.tab();
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_BACKUP
			SDC.theFile.print(rec.mainB.info());			SDC.tab();
#endif // USE_MAIN_BACKUP
#if USE_DROGUE_BACKUP
			SDC.theFile.print(rec.drogB.info());			SDC.tab();
#endif // USE_DROGUE_BACKUP
#endif // LOG_IGNITER_HEALTH

#if LOG_EMERGENCY
			SDC.theFile.print(rec.mainN.getEmergency());	SDC.tab(); //Estado de emergencia
#endif // LOG_EMERGENCY

#if USE_APOGEE_DETECTION
			//if (apg.getApogeu(0.9, 0))
			if (Gutil.mem)
			{
				if (SDC.util.oneTime())
				{
					SDC.theFile.print(F("H:"));
					SDC.theFile.print(apg.getApgPt());
					SDC.theFile.print(F(" m\tT:"));
					SDC.theFile.print(apg.getApgTm());
					SDC.theFile.print(F(" s"));
					SDC.tab();
				}
#if USE_DEPLOY
				if (rec.mainN.getState(0))
				{
#if USE_LoRa
					rec.mainN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act MainN:"));
					SDC.theFile.print(rec.mainN.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}

#if USE_DUAL_DEPLOY
				if (rec.drogN.getState(0))
				{
#if USE_LoRa
					rec.drogN.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act DrogueN:"));
					SDC.theFile.print(rec.drogN.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
				if (rec.mainB.getState(0))
				{
#if USE_LoRa
					rec.mainB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act MainB:"));
					SDC.theFile.print(rec.mainB.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
				if (rec.drogB.getState(0))
				{
#if USE_LoRa
					rec.drogB.getStateReset(); // Serial -> SD -> LoRa
#endif // USE_LoRa
					SDC.theFile.print(F("Act DrogueB:"));
					SDC.theFile.print(rec.drogB.getDeploymentHeight());
					SDC.theFile.print(F("m\t"));
				}
#endif // USE_DROGUE_BACKUP


#endif // USE_DEPLOY

			}
#endif // USE_APOGEE_DETECTION
			SDC.theFile.println();
			SDC.close();
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
			sysC++;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT

		}
		else
		{
			SDC.util.mem = 1;
#if USE_MORSE_MSG
			if(!mensageiro.getQuiet()) mensageiro.msgAux += " S"; // ...
#endif  // USE_MORSE_MSG
		}
	}
	else{
		if (SDC.util.eachT(15)) if (SDC.begin()) SDC.util.mem = 0;
		#if USE_MORSE_MSG
			if(!mensageiro.getQuiet()) mensageiro.msgAux += " S"; // ...
		#endif  // USE_MORSE_MSG
	}
}
#endif // USE_STORAGE

//////////////////////////////////////////////////////BZZ//////////////////////////////////////////////////////

#if USE_BEEP_SYSTEM_COUNT
inline void beep(unsigned int N)
{
	if (beeper.eachT(holdT*SYSTEM_COUNT * 4) || beeper.oneTime())
	{
		beeper.mem = BUZZER_ON_LEVEL;
		beeper.counterReset();
		beeper.forT(holdT);
	}
	if (beeper.getCount() < (N + 1) * 2) if (!beeper.forT())
	{
		digitalWrite(BUZZER_PIN, beeper.mem);
		beeper.mem = !beeper.mem;
		beeper.counter();
		beeper.forT(holdT);
	}
}

inline void beep()
{
	digitalWrite(BUZZER_PIN, !BUZZER_ON_LEVEL);
	beeper.counterReset();
}
#endif // USE_BEEP_SYSTEM_COUNT

//////////////////////////////////////////////////////LRM//////////////////////////////////////////////////////

#if USE_LoRa
inline void LoRaSend()
{
#if USE_LoRa_E32_settable
	if (configLoRa.isTelemetryPaused()) return;
#endif // USE_LoRa_E32_settable

	/*
	L - Line
	T - Time
	A - Latitude
	O - Longitude
	h - horas
	n - minutos
	g - precisão
	H - altura atual
	s - SD
	a - altura Apogeu
	t - tempo Apogeu
	M - Main Normal
	D - Drogue Normal
	m - Main Backup
	d - Drogue Backup
	c - Temperatura
	*/
	if (LRutil.eachT(LoRaDelay))
	{
		LoRa.println();
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_LINE)); // >>L<<ine
#endif // USE_LoRa_KEYVALUE
		LoRa.print(LRutil.counter());
		LoRa.print('\t');
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_TIME)); // >>T<<ime
#endif // USE_LoRa_KEYVALUE
		LoRa.print(LRutil.sinceBegin());
		LoRa.print('\t');
#if USE_GNSS
		if(GpS.util.mem) { // Somente mandar dados validos apos primeira leitura bem sucedida
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_LAT)); // l>A<titude
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getLatitude(), 6);//Latitude
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_LON)); // l>O<ngitude
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getLongitude(), 6);//Longitude
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_HOUR)); // >>h<<ora
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getHour());//Hora
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_MIN)); // mi>>n<<uto
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getMinute());//Minuto
			LoRa.print('\t');
#if USE_LoRa_KEYVALUE
			LoRa.print(F(LoRa_KEY_PREC)); // Presicão >>g<<
#endif // USE_LoRa_KEYVALUE
			LoRa.print(GpS.getPrecision());//Precisao
			LoRa.print('\t');
		} else {
			LoRa.print(F(
#if USE_LoRa_KEYVALUE
				LoRa_KEY_LAT
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_LON
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_HOUR
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_MIN
#endif // USE_LoRa_KEYVALUE
				"~\t"
#if USE_LoRa_KEYVALUE
				LoRa_KEY_PREC
#endif // USE_LoRa_KEYVALUE
				"~\t"
			));
		}
#endif // USE_GNSS
#if USE_APOGEE_DETECTION
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_HEIGTH)); // >>H<<eight
#endif // USE_LoRa_KEYVALUE
		LoRa.print(apg.getHeight());
		LoRa.print('\t');
		// LoRa.print(apg.getSigma(), 3);
		// LoRa.print('\t');
#endif // USE_APOGEE_DETECTION
#if USE_STORAGE
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_SD)); // >>s<<D
#endif // USE_LoRa_KEYVALUE
		LoRa.print(!SDC.util.mem);
		LoRa.print('\t');
#endif // USE_STORAGE
#if USE_APOGEE_DETECTION
		if (Gutil.mem)
		{
			LoRa.print(F(LoRa_KEY_APG_HEIGHT)); // >>a<<pogee height
			LoRa.print(apg.getApgPt());
			LoRa.print(F("\t" LoRa_KEY_APG_TIME)); // apogee >>t<<ime
			LoRa.print(apg.getApgTm());
			LoRa.print('\t');
		}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_APG_HEIGHT
#endif // USE_LoRa_KEYVALUE
		"~\t"
#if USE_LoRa_KEYVALUE
		LoRa_KEY_APG_TIME
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // USE_APOGEE_DETECTION
		//LRutil.oneTimeReset();

#if USE_DEPLOY
	if (
#if USE_LoRa_CONTIGUOUS
		rec.mainN.getGlobalState(1)
#else
		rec.mainN.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
		// rec.mainN.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_MAIN_NORMAL)); // >>M<<ain normal
		LoRa.print(rec.mainN.getDeploymentHeight());
#else
		LoRa.print(F("Act MainN:"));
		LoRa.print(rec.mainN.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_MAIN_NORMAL
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS

#if USE_DUAL_DEPLOY
	if (
#if USE_LoRa_CONTIGUOUS
		rec.drogN.getGlobalState(1)
#else
		rec.drogN.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_DROGUE_NORMAL)); // >>D<<rogue normal
		LoRa.print(rec.drogN.getDeploymentHeight());
#else
		// rec.drogN.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
		LoRa.print(F("Act DrogueN:"));
		LoRa.print(rec.drogN.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_DROGUE_NORMAL
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
	if (
#if USE_LoRa_CONTIGUOUS
		rec.mainB.getGlobalState(1)
#else
		rec.mainB.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_MAIN_BACKUP)); // >>m<<ain backup
		LoRa.print(rec.mainB.getDeploymentHeight());
#else
	// rec.mainB.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
		LoRa.print(F("Act MainB:"));
		LoRa.print(rec.mainB.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_MAIN_BACKUP
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
	if (
#if USE_LoRa_CONTIGUOUS
		rec.drogB.getGlobalState(1)
#else
		rec.drogB.getState(0)
#endif // USE_LoRa_CONTIGUOUS
	)
	{
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_DROGUE_BACKUP)); // >>d<<rogue Backup
		LoRa.print(rec.drogB.getDeploymentHeight());
#else
		// rec.drogB.getStateReset(); // Serial -> SD -> LoRa // Último a ser realizado, não reseta
		LoRa.print(F("Act DrogueB:"));
		LoRa.print(rec.drogB.getDeploymentHeight());
		LoRa.print(F("m\t"));
#endif // USE_LoRa_KEYVALUE
	}
#if USE_LoRa_CONTIGUOUS
	else  LoRa.print(F(
#if USE_LoRa_KEYVALUE
		LoRa_KEY_DROGUE_BACKUP
#endif // USE_LoRa_KEYVALUE
		"~\t"
	));
#endif // USE_LoRa_CONTIGUOUS
#endif // USE_DROGUE_BACKUP

#endif // USE_DEPLOY
#if USE_BARO
#if USE_LoRa_KEYVALUE
		LoRa.print(F(LoRa_KEY_TEMPERATURE)); // >>c<< temperatura
#endif // USE_LoRa_KEYVALUE
		LoRa.print(baro.getTemperature());
		LoRa.print('\t');
#endif // USE_BARO
  }
}
#endif // USE_LoRa

//////////////////////////////////////////////////////COM//////////////////////////////////////////////////////

#if HAS_COMM_CHANNEL
template <typename T> void transmit(T message)
{
#if USE_PRINT
	Serial.print(message);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.print(message);
#endif // USE_LoRa
}

template <typename T> void transmitln(T message)
{
#if USE_PRINT
	Serial.println(message);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.println(message);
#endif // USE_LoRa
}

template <typename T, typename R> void transmit(T message, R value)
{
#if USE_PRINT
	Serial.print(message, value);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.print(message, value);
#endif // USE_LoRa
}

template <typename T, typename R> void transmitln(T message, R value)
{
#if USE_PRINT
	Serial.println(message, value);
#endif // USE_PRINT
#if USE_LoRa
	LoRa.println(message, value);
#endif // USE_LoRa
}

#endif // HAS_COMM_CHANNEL

//////////////////////////////////////////////////////RET//////////////////////////////////////////////////////

inline void readEverything()
{
#if USE_VIRTUAL
	Bridge.update();
#endif // USE_VIRTUAL
#if USE_BARO
	if (baro)
	{
		MM_baro[0] = baro.getTemperature();
		MM_baro[1] = baro.getPressure();
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
		sysC++;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
		baroHasData = true;
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " B"; // -...
#endif  // USE_MORSE_MSG
		baroHasData = false;
	}
#endif // USE_BARO
#if USE_ACCEL
	if (accel)
	{
		MM_accel[0] = accel.getX();
		MM_accel[1] = accel.getY();
		MM_accel[2] = accel.getZ();
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
		sysC++;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " A"; // .-
#endif  // USE_MORSE_MSG
	}
#endif // USE_ACCEL
#if USE_GYRO
	if (giro)
	{
		MM_giro[0] = giro.getX();
		MM_giro[1] = giro.getY();
		MM_giro[2] = giro.getZ();
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
		sysC++;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " G"; // --.
#endif  // USE_MORSE_MSG
	}
#endif // USE_GYRO
#if USE_MAGN
	if (magn)
	{
		MM_magn[0] = magn.getX();
		MM_magn[1] = magn.getY();
		MM_magn[2] = magn.getZ();
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
		sysC++;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " M"; // --
#endif  // USE_MORSE_MSG
	}
#endif // USE_MAGN
#if USE_GNSS
	if (GpS) GpS.util.forT(10);
	if (!GpS.util.mem) if (GpS.isNew()) GpS.util.mem = true; // Auxiliar de primeira leitura bem sucedida
	if (GpS.util.forT()) {
#if USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
		sysC++;
#endif // USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT || FORCE_SYSTEM_COUNT
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " L"; // .-..
#endif  // USE_MORSE_MSG
}

#endif // USE_GNSS
#if USE_DEPLOY && (USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT)
	if (rec.mainN.info()) {
		sysC++;
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rmn"; // .-. -- -.
#endif  // USE_MORSE_MSG
	}

#if USE_DUAL_DEPLOY
	if (rec.drogN.info()) {
		sysC++;
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rdn"; // .-. -.. -.
#endif  // USE_MORSE_MSG
	}
#endif // USE_DUAL_DEPLOY

#if USE_MAIN_BACKUP
	if (rec.mainB.info()) {
		sysC++;
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rmb"; // .-. -- -...
#endif  // USE_MORSE_MSG
	}
#endif // USE_MAIN_BACKUP

#if USE_DROGUE_BACKUP
	if (rec.drogB.info()) {
		sysC++;
	} else {
#if USE_MORSE_MSG
		if(!mensageiro.getQuiet()) mensageiro.msgAux += " Rdb"; // .-. -.. -...
#endif  // USE_MORSE_MSG
	}
#endif // USE_DROGUE_BACKUP

#endif // USE_DEPLOY && (USE_SOFTWARE_REMOVE_BEFORE_FLIGHT || USE_WAIT_UNTIL_FLIGHT)
}

#if USE_STORAGE
inline void printHeaderSD()
{

#if ARDUINO_ARCH_ESP32
	SPI_SD.begin(SCK_SD_ESP, MISO_SD_ESP, MOSI_SD_ESP, CS_SD_ESP);
#endif // ARDUINO_ARCH_ESP32
	SDC.begin();
	if (SDC)
	{
#if HAS_COMM_CHANNEL
		transmit(F("\nSD start OK "));
		transmit(SDC.getFname());
#endif // HAS_COMM_CHANNEL

		//////////////////File Header//////////////////

#if LOG_RESET_REASON && defined(ARDUINO_ARCH_ESP32)
		SDC.theFile.print(F("Reset reason:\t"));
		SDC.theFile.print(int(esp_reset_reason()));
		SDC.theFile.print(F(" <"));
		SDC.theFile.print(resetReasonName(esp_reset_reason()));
		SDC.theFile.println(F(">"));
#endif // LOG_RESET_REASON && defined(ARDUINO_ARCH_ESP32)
#if USE_APOGEE_DETECTION
		SDC.theFile.print(F("Liftoff flag:\t"));
		SDC.theFile.print(apg.getLiftoff());
		SDC.theFile.println();
		SDC.theFile.print(F("Start at:\t"));
		SDC.theFile.print(apg.getZero());
		SDC.theFile.print(F("\tm\tZero spread:\t"));
		SDC.theFile.print(apg.getSpread());
		SDC.theFile.print(F("\tm"));
		if(apg.getFixZero()) {
			SDC.theFile.print(F("\t(Using EEPROM Zero Ref @ < 0x"));
			SDC.theFile.print(apg.getEEAddress(), HEX);
			SDC.theFile.print(F(" >!) "));
		}
		SDC.theFile.println();
#endif // USE_APOGEE_DETECTION

		///////////////////////////////////////////////

		SDC.theFile.println(F(
			"tempo.s\t"
#if USE_ACCEL
			"mps2.x.accel\tmps2.y.accel\tmps2.z.accel\t"
#endif // USE_ACCEL
#if USE_GYRO
			"dps.x.gyros\tdps.y.gyros\tdps.z.gyros\t"
#endif // USE_GYRO
#if USE_MAGN
			"uT.x.magn\tuT.y.magn\tuT.z.magn\t"
#endif // USE_MAGN
#if USE_BARO
			"c.baro\tpa.baro\t"
#endif // USE_BARO
#if USE_APOGEE_DETECTION
			"m.h.baro\t"
#endif // USE_APOGEE_DETECTION
#if LOG_GPS
			"lat.GPS\tlon.GPS\tm.h.GPS\tmps.GPS\tsat.GPS\tprec.GPS\tnew.GPS\t"
#endif // LOG_GPS
#if LOG_APOGEE
			"alpha.apg\tsigma.apg\t"
#endif // LOG_APOGEE
#if LOG_DEPLOY_EVERY_LINE
			"on.mainN\tfire.mainN\t"
#if USE_DUAL_DEPLOY
			"on.drogN\tfire.drogN\t"
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_BACKUP
			"on.mainB\tfire.mainB\t"
#endif // USE_MAIN_BACKUP
#if USE_DROGUE_BACKUP
			"on.drogB\tfire.drogB\t"
#endif // USE_DROGUE_BACKUP
#endif // LOG_DEPLOY_EVERY_LINE

#if LOG_IGNITER_HEALTH
			"ok.mainN\t"
#if USE_DUAL_DEPLOY
			"ok.drogN\t"
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_BACKUP
			"ok.mainB\t"
#endif // USE_MAIN_BACKUP
#if USE_DROGUE_BACKUP
			"ok.drogB\t"
#endif // USE_DROGUE_BACKUP
#endif // LOG_IGNITER_HEALTH

#if LOG_EMERGENCY
			"emrg\t"
#endif // LOG_EMERGENCY


		));

		///////////////////////////////////////////////
/*
		SDC.theFile.println(F(
			"seg\t"
#if USE_ACCEL
			"X\tY\tZ\t"
#endif // USE_ACCEL
#if USE_GYRO
			"X\tY\tZ\t"
#endif // USE_GYRO
#if USE_MAGN
			"X\tY\tZ\t"
#endif // USE_MAGN
#if USE_BARO
			"C\tPascal\t"
#endif // USE_BARO
#if USE_APOGEE_DETECTION
			"m\t"
#endif // USE_APOGEE_DETECTION
#if USE_GNSS
			"Latitude\tLongitude\tAltitude (m)\tspeed\tSat\tPrec\t"
#endif // USE_GNSS
		));
*/
		///////////////////////////////////////////////

		SDC.close();
	}
	else
	{

#if HAS_COMM_CHANNEL
		transmit(F("\nSD err"));
#endif // HAS_COMM_CHANNEL
	}
}
#endif // USE_STORAGE


#if PRINT_ANY
inline void printHeaderSerial()
{

	Serial.println(F(
#if PRINT_LOOP_COUNT
	"loop\t"
#endif // PRINT_LOOP_COUNT

#if PRINT_PERFORMANCE_TIMING
	"temp\t"
#endif // PRINT_PERFORMANCE_TIMING

#if PRINT_TIME
	"tempo.s\t"
#endif // PRINT_TIME


#if PRINT_ACCEL_X
	"mps2.x.accel\t"
#endif // PRINT_ACCEL_X
#if PRINT_ACCEL_Y
	"mps2.y.accel\t"
#endif // PRINT_ACCEL_Y
#if PRINT_ACCEL_Z
	"mps2.z.accel\t"
#endif // PRINT_ACCEL_Z


#if PRINT_GYRO_X
	"dps.x.gyros\t"
#endif // PRINT_GYRO_X
#if PRINT_GYRO_Y
	"dps.y.gyros\t"
#endif // PRINT_GYRO_Y
#if PRINT_GYRO_Z
	"dps.z.gyros\t"
#endif // PRINT_GYRO_Z


#if PRINT_MAGN_X
	"uT.x.magn\t"
#endif // PRINT_MAGN_X
#if PRINT_MAGN_Y
	"uT.y.magn\t"
#endif // PRINT_MAGN_Y
#if PRINT_MAGN_Z
	"uT.z.magn\t"
#endif // PRINT_MAGN_Z


#if PRINT_BARO_TEMPERATURE
	"c.baro\t"
#endif // PRINT_BARO_TEMPERATURE
#if PRINT_BARO_PRESSURE
	"pa.baro\t"
#endif // PRINT_BARO_PRESSURE

#if PRINT_HEIGHT
	"m.h.baro\t"
#endif // PRINT_HEIGHT
#if PRINT_PEAK_HEIGHT
	"m.max.apg\tt.max.apg\t"
#endif // PRINT_PEAK_HEIGHT
#if PRINT_APOGEE_ALPHA
	"alpha.apg\t"
#endif // PRINT_APOGEE_ALPHA
#if PRINT_APOGEE_SIGMA
	"sigma.apg\t"
#endif // PRINT_APOGEE_SIGMA
#if PRINT_APOGEE_SIGMA_MAX
	"max.sigma.apg\t"
#endif // PRINT_APOGEE_SIGMA_MAX

#if PRINT_GNSS
	"Lat.GPS\tLon.GPS\tm.h.GPS\tmps.GPS\tsat.GPS\tprec.GPS\tchars.GPS\tdate.GPS\ttime.GPS\tKph.GPS\t"
#endif // PRINT_GNSS

#if PRINT_RECOVERY_HEALTH
	"ok.mainN\t"
#if USE_DUAL_DEPLOY
	"ok.drogN\t"
#endif // USE_DUAL_DEPLOY
#if USE_MAIN_BACKUP
	"ok.mainB\t"
#endif // USE_MAIN_BACKUP
#if USE_DROGUE_BACKUP
	"ok.drogB\t"
#endif // USE_DROGUE_BACKUP
#endif // PRINT_RECOVERY_HEALTH

	));

/*
	Serial.println(F(
#if PRINT_LOOP_COUNT
	"\t"
#endif // PRINT_LOOP_COUNT

#if PRINT_PERFORMANCE_TIMING
	"s\t"
#endif // PRINT_PERFORMANCE_TIMING

#if PRINT_TIME
	"s\t"
#endif // PRINT_TIME


#if PRINT_ACCEL_X
	"X\t"
#endif // PRINT_ACCEL_X
#if PRINT_ACCEL_Y
	"Y\t"
#endif // PRINT_ACCEL_Y
#if PRINT_ACCEL_Z
	"Z\t"
#endif // PRINT_ACCEL_Z


#if PRINT_GYRO_X
	"X\t"
#endif // PRINT_GYRO_X
#if PRINT_GYRO_Y
	"Y\t"
#endif // PRINT_GYRO_Y
#if PRINT_GYRO_Z
	"Z\t"
#endif // PRINT_GYRO_Z


#if PRINT_MAGN_X
	"X\t"
#endif // PRINT_MAGN_X
#if PRINT_MAGN_Y
	"Y\t"
#endif // PRINT_MAGN_Y
#if PRINT_MAGN_Z
	"Z\t"
#endif // PRINT_MAGN_Z


#if PRINT_BARO_TEMPERATURE
	"C\t"
#endif // PRINT_BARO_TEMPERATURE
#if PRINT_BARO_PRESSURE
	"Pascal\t"
#endif // PRINT_BARO_PRESSURE

#if PRINT_HEIGHT
	"m\t"
#endif // PRINT_HEIGHT
#if PRINT_PEAK_HEIGHT
	"Max h\tMax t\t"
#endif // PRINT_PEAK_HEIGHT
#if PRINT_APOGEE_ALPHA
	"Alpha\t"
#endif // PRINT_APOGEE_ALPHA
#if PRINT_APOGEE_SIGMA
	"Sigma\t"
#endif // PRINT_APOGEE_SIGMA
#if PRINT_APOGEE_SIGMA_MAX
	"Max S\t"
#endif // PRINT_APOGEE_SIGMA_MAX

#if PRINT_GNSS
	"Latitude\tLongitude\tAltitude\tSpeed (m/s)\tSat\tPrec\t"
#endif // PRINT_GNSS

	));
*/
}
#endif // PRINT_ANY

#pragma endregion

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
/*
 *                                                                           I8$$$????$?????????$??????$$?8+
 *                                                                    $$$$?????????????????????????????????????$?$$
 *                                                              +?$???????????????????????????????????????????????????$$:
 *                                                          ?????????????????????$?$??$O???????$$$$????$?????????????????????
 *                                                      $$????????????????$$?,           ?????$$           ,$$$??????????????$$$I
 *                                                  ,$8?????????????$$,                  ?????$$                   $$$????????????$?
 *                                                $?????????????8                        ?????$$                        $?????????????$
 *                                             8??$????????$$                            ?????$$                            $$?????????$??
 *                                          +8??????????$                                ?????$$                               :$?????????I$,
 *                                        $8?????????$                                   ??????$                                   $???????$?$?
 *                                      $?????????$                              I$8$$????????$$????8$?+                             ,$????????$$
 *                                    $????????$8                          88???????????????????????????????88:                         $$????????8
 *                                  $???????$?,                      I$$???????????????????????$$$$????????????????+                      =?$????????
 *                                $?????$$?$                     $???????????????$?????????????,  $????????????????????$                     ?$$??????$
 *                              8$???????$                   ?$????????$$???????????????    ???   ?$I$$???????????????????8+                   $??????$$?
 *                            =$$??????$                  $??????$$=      $???     $?$   $?  I?   ?   $?$I   $???????$$??$?$?$I                  $$?????$?,
 *                           $???????8                 I$???????$I  =8$,  $??  $?   $$  ???????      $??   8I  8?$$   ?????????$?~                 8????????
 *                         $$$??????                 8$???????????   8,  :$?$  ??$  $$   $?  ~?   $   $$       ~?     $???????????I8                 ????????I
 *                        $???????:               ?$??????????????:        ??       ??$I    8?$  I?I  I$  8?$8?$?8  ~ ???????$:8????$$~               +$?$????8
 *                      ~??????$$               $????$8+ 8????????8   ?$    I$$??$$$????????????????????$~    =?$   $????????I   ~$????$+               8??????$
 *                     $?????????$+           $$?????     $?$???????  ????$????????????$?????????$??????????????$   $???????:   ,  :$?????+            $?????????$
 *                    $????????????$        =$???$?$  $$???$$??????$ ?$?????$????$$$$I,           :?$$$????$?????$? 8??????~   ??8  +??????$,        8????????????$
 *                   ??????????????$?8     ?$$?????   $?$??  $?????$????????$I                             I$??????$$$????,   $?$   +????????$     =$???????????????
 *                 ,$?????$  $?????????  8??????????   $??8  $?????????$$=                                     I8????????8   ?$??   $?????????$?  8$????????+~$??????
 *               ?$?????$     $??????????$??    $??        ,$??????$?                                             $???????$       ?$$$8????????$$????????8    ???????,
 *               ???????$       ???????????I     ?8$?8     $$???$?,                                                   +$?????8$,:$?8      $??????????????      ,???????
 *             ,8?????$O          O?????$8   $$,   ,+?????????$~                                                         I???$?????    $$  ???????????$          ??????$=
 *             +??????8            =?????$      $$   ?????$??                                                               ???????  ?O    $????????$I            ???????
 *            ???????$            8????????,   ,$?  8????$$                                                                   $????$   8? 8?$$$???????$            8?????$
 *            $?????$            I$?????????$      ?$???$ ,                                                                     $?????~ =$?I   ,$??????$            $???$?$
 *           ???????            $?$?$  ??????$=  8????$                                    $8:                                    $????$? ?$  ?  ???????$            $?????$
 *          $?????$            $???8         8$$$???$:                                    :O88~                                    $$????, $=  8 $???$???$            $?????$
 *         $$????$            $????I   $I~    $$???8                 8                    O8888                          =           8???$  :  ????$  ????$            $?????8
 *        I??????            8??????$   8  $?8???$,                 OO                   ?888888                        ,8            ?$???8  ?$?+  8 $$???$           ,??????
 *        ?????$~           $???~=????    $$?????,                  + O                  8888888                       ?888~           +?$???I$~    ?$??????I           ??????$
 *       ???????           $???8  $??I?   ??????                                        =88888888                       888              ?$??$    ????????????           ??????8
 *      ~?????$           ,???$  $$  8?$  ?$??8                                         88888888O                                         ?????I??$    $??????            $?????
 *      ??????:           $??$   I  ?$???$????                                          888888888                                          $???$8       $?????$           ??????$
 *     $??????           $???$      ?$$?????8                                           888888888                                           $?  $  ??$ +,??????$           $?????+
 *     $?????           ???????$?      ???$8                                            888888888,                                           8? $?~ ,    ???????           ?$$???$
 *    ?$????$           $??$ 8$$???? ,+???$                                             888888888~                                            ?,       $????????$           $$????,
 *    ??????           ????      ~????????            ~O                                888888888?                                             $= , $???$  ??????$          ,$????$
 *   $??????$?         ????$$=     ,????$                                               888888888?         =O                                  ????$I      ???????         $$??????:
 *   ??????????$      $?$????$?$$~ ,$?$??                                               888888888?                                              $????  ,$?  ??????$     ~$??????????
 *   $????????????$   ??$?  ?$???8  ????                                                888888888?                                               $??$$??$:  ??????$  :$?????????????
 *  $???????????????$$???          ????=                                                888888888$                                               $??$I      $??????$$???????????????:
 *  $????????????????????$$$      +????                                  OO             888888888$                          =                     ???$   8$$??????????????????:?????$
 *  $????$ $???????????????????$??????8                                                 888888888$                        8888=                   $??????????????????????$$    ?????$
 * I??????    $$??????????????????????                                                  888888888$                         OO?                     ????????????????????$~      $?????
 * $?????       :$$?$????????????????$                                                  888888888$                                                 $?????????????????I         +??????
 * ??????           ?????????????????,                                                  8888,$888$                                                 $?????????????????           ?????$
 * ?????$          +????????????????$                                                 ? 8888 8888$$                                                 ?????????????????           ??????
 * ??????          $????????????????$                                                 8,8888 8888$8?                                                ?????????????????=          $????$
 * ?????8          $????????????????$                                                88:8888 8888$88:                                               $????????????????$          8????$
 * ?????$          $????????????????=                                               888:8888 8888$888                                               $?????????????????          $?????,
 * ?????I          ?$???????????????                                               ,888:8888 8888$8888                                              +????????????????$          ?????$+
 * ??????          ?????????????????                  :O8                          8888:8888 $888$88888                              ,,             =????????????????$          $????$I
 * ??????          ?????????????????                                              88888:8888 8888$88888                             O$:             ,$???????????????$          $??????
 * ??????          ?????????????????                                              88888:8888 8888$88888            8               $??              +????????????????$          $????$$
 * ??????          $$???????????????                                              88888 O888 $888 I8888          I8?                                =$????????????????          $????$+
 * ?????$          $????????????????~                                             888O ,? ~8,$+ :: $888                                             $?????????????????          $????$,
 * $????$          ?$??????????????$?                                             888, 888$$~88888: $88                                             $????????????????$          $?????
 * ?????$          $????????????????$                                             88?  88888?88888O ,88                                             ?????????????????+          $?????
 * ?????$          +$???????????????$                                             88   88888$88888+  ?8                                             ?????????????????           ??????
 * $?????           $????????????????                                             8    ~8888888888,   $                                            I????????????????$           ?????$
 * $?????           $????????????????8                                                 ,O8888888O$                                                 ?????????????????$          I??????
 * I??????$?$8      $?????????????????          ,$,$                                    8,O88$$==O                                                 ?????????????????$    +$?$????????
 *  ?????????$??$?$? ????????????????$8         888,               +?                  8I$88888 $O                                                ???????????????????$$?$???????????$
 *  ?$?????????????????????????????????           ,                                   8,?? 888:=?+?                       :O$                     $?????????????????????????????????$
 *  $??????????????????????????????????~                                              $$?$$$8O?8?$$                       O8OI                   $?????????????????????????????????$=
 *   ??????+$8$?????????????????????????                                              $????$$888 8$?                       ?                     ?????????????????????$??$$$  $?????
 *   ?????$,     ???$???????????????????8                                             $????$?8$$???I                                            8$?????????????????$$I        ??????
 *   $?????$           ??????????????????                                             $????$I ?????$                                           ~??????????????????           $?????=
 *    ??????           $?????????????????$                                            $????~I$?????$                                          ,$?????????????????$           ??????
 *    $$????8           $??????????????????                                           ??????????????                                          $?????????????????$           $?????=
 *     $?????           ?$?????????????????$                                         :?$????????????                                         $??????????????????:          =?????$
 *     $?????$           $??????????????????$                                        ?$?????????????:                                       ???????????????????$           $??????
 *      ??????            ????????????????????                                       ???????????????$                                      $???????????????????           ?????$$
 *      +?????$           =???????????????????8                                      ???????????????$                                     $??????????????????$            $????$
 *       ??????8           $$??????????????????$                                    $???????????????$~                                   $????????????????????           $?????$
 *        $$???$            ?????????????????????                                   $?????????????????                                  $???????????????????$           I?????$
 *        ?$???$?            $???????????????????$                                 +??????????????????                                ,????????????????????$           ,$?????~
 *         8?????$            $???????????????????$$                               $??????????????????$                              $?$??????????????????$            ??????$
 *          $?????$            $????????????????????$                             $?????????????????????                           ~?????????????????????$            $?????$
 *           ??????8            ?????????????????????$8                          ???????????????????????                          $$????????????????????$            $?????$
 *           ,$????$$            $???????????????8??????$                       +??????????????????????$$                       ?$?????????????????????$$$$$$$$$$???$??????
 *            ????????????????????????????????$$,  +$?????8                    8?????????????????????????$                    $?????????$  $??????????????????????????????=
 *             ?$?????????????????????????????8  ,?  ?$?????$                 $????????????????????????????$                $?????????? ,$  ? ???????????????????????????,
 *              8???????????????????????????$     ?  $$???????$$           ,$??????????????????????????????$??~         ????????????$~~+8 $? 8$??????????????????????????
 *               I$?????????????????????????$:+$$?, 8$??????????$?$$?$$$?$???$???????????????????????????????????????$??$?????????$?$ $??  :??$???$             $??????~
 *                8??????$             ~?????????$$  8 $$??????????????????????????????????????????????????????????????????????,8?+  ,??$$??????$              $$?????+
 *                 =???????              $???????? ,I : ?+$$???????????????????????????????????????????????????????????????????  8$$?  $???????$              $?????$
 *                  ,???????              :$?????$$ $?:OI ?$$????????????????????????????????????????????????????????$I?????$  $  ??:I8??????$              +???????:
 *                   ,?$?????$              8$??????$ $~ ~$::$??$+  $???????????????????????????????????????????????8   $??$ 8+I$   8??????8=              $??????$
 *                     ???????$               8$??????$ +???????  ? $????????????????????????????????????????????$??$ :   $$=:$  ?????????$              ,$??????8
 *                      ??????$??               8??????I$????$  ,?+ $?????????????????????????????????????????$  ???$  ?$  =$  ?$???????$               8??????$~
 *                        $????$??                ?$????????   ?,   $?   ???????????????????????????$?  $????$?  $??$    $~ :$$$?????$I                ???????$ ,
 *                         8???????$                ~$???????$$??$: $$  88$    8???$????????????????$    $?$  O$ ???$  $??$???????$$                 $???????8
 *                           ????????$                 $I?????????$ $ 8$ $= $$  $  I +$  8 ??   ?$??8 ??  $?  ??  $?? 8???????????$$$?             $???????$
 *                            ?????????I            =$??$I$$?????????$$:?$  ?8  ?     $  I??8$$ =???       ?$   ?=$???????????????????$$?        $????????+
 *                              $$$?????$?        $????$$?$$$8?$?????????$ $$ ??$ ,$$I$ :$?$ ?$ ~??$  $$?? ,$????????????$8$ $????????????$?   $???????$$
 *                                $$???????I   $???????????,    ~??$$????  $??????$$$$$+???$~ ? ,$?$?$????????????????$$,      ,$I????????????????????8
 *                                  $???????????????????8           :8$$??????????????????????????????????????????$$              ~$?????????????????
 *                                    8???????????????:                 $=?$$???????????????????????????????????$$                   ?$???????????$
 *                                     +$??????????$                   $??$?$?? ?8$??????????????????$88=  ?$??????                   8??????????
 *                                        $??????????I                ????????+           $$$????          ,8?????$$               $?????????$$
 *                                          $$?????????$$            8??????$+            $?????$            ?$?????$           $$??????$?$$?
 *                                             $??????????$$+       $??????$$             ??????$             $?????$$      I$???????????$
 *                                               =??$?$????????$=  ????????8              ???????,             $??????$ I$???????????$$
 *                                                  ?$$?$??????????????????               ???????+             ,??????????????????$?:
 *                                                      $$???????????????$?$,             ???????$          ,$$????????????????$$
 *                                                         :$????????????????$?????$$?$$I=$??????$$$$$?$???$???????????????$$,
 *                                                              $$?????????????????????????????????????????????????????$$
 *                                                                   ?$??????????????????????????????????????????$8=
 *                                                                          ?8$????????????????????????????8~
 *                                                                                    ,,+++??$+++,
 */
#endif
