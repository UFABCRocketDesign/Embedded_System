// MonoDeploy.h

#pragma once

#ifndef _MONODEPLOY_h
#define _MONODEPLOY_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#if SERVO_MODE
#include <Servo.h>
#endif // SERVO_MODE

class MonoDeploy
{
	static const bool command = HIGH;	// Active state
	static bool apogee;					// Internal representation of the apogee state
	static unsigned long TimeZero;		// Zero reference for timer (liftoff)
	static unsigned long Tseal;			// Seal momment time
	static float height;				// Internal representation of the height

	const unsigned long Tign;	// Active time
	const unsigned int cPin;	// Command pin
	const unsigned int iPin;	// Info pin
	const unsigned int sysMode;	// system mode: Mode 0 - picotechnic; Mode 1 - servo motor
#if SERVO_MODE
	Servo motor;
#endif
	unsigned long Tnow; // Current time related to liftoff
	unsigned long Tcmd; // Time when deployment start
	unsigned long Theight; // Time when reached deployment height
	unsigned long Tmax; // Time force apogee state if useM setted
	float cmdHeight; // Deployment height
	unsigned long cmdDelay; // Time to delay deployment
	bool useH = false; // Set to use hight based deployment
	bool useT = false; // Set to use delayed deployment
	bool useM = false; // Set to force apogee state after Tmax seconds from resatTimer
	bool useH_A = false; // Conditional height reached
	bool useT_A = false; // Conditional time reached
	bool cmdSeal = false; // Command seal, to avoid reprocessing
	bool stateAux = false; // Current deplyment state
	bool globalState = true; // Current global deployment state (true after deployment)
	bool globalStateAux = false; // Aux to processe Current global deployment state
	bool sPin = !command; // Current output sate

	bool emergencyState = false; // Set to emergency state
	bool useH_EM = false; // To save previous configuration
	bool useT_EM = false; // To save previous configuration
	unsigned long cmdDelay_EM; // To save previous configuration

public:
	MonoDeploy(unsigned int commandPin, unsigned int infoPin, unsigned long actionTime = 5, unsigned int systemMode = 0);

	static void resetTimer();
	static void sealApogee(bool apg);
	static bool getApogee();
	static void putHeight(float H);

	void setHeightCmd(float H);
	void setDelayCmd(float T);
	void setTmax(float T);
	bool begin();
	bool info();
	bool getState(bool type = 1);
	bool getGlobalState();
	void refresh();

	void emergency(bool state, float T_EM);
};

#endif