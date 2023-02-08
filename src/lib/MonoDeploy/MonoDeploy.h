// MonoDeploy.h

#pragma once

#ifndef _MONODEPLOY_h
#define _MONODEPLOY_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#if SERVO_MODE
#include <Servo.h>
#endif // SERVO_MODE

class MonoDeploy
{
	static const bool command = HIGH; // Active state
	static bool apogee;
	static unsigned long TimeZero; // Zero reference for timer
	static unsigned long Tseal;	   // Seal momment time
	static float height;

	const unsigned long Tign; // Active time
	const unsigned int cPin;  // Command pin
	const unsigned int iPin;  // Info pin
	const unsigned int sysMode;
#if SERVO_MODE
	Servo motor;
#endif
	unsigned long Tnow;
	unsigned long Tcmd;
	unsigned long Theight;
	unsigned long Tmax;
	float cmdHeight;
	float cmdDelay;
	bool useH = false; // Command height based
	bool useT = false; // Command time based
	bool useM = false;
	bool useH_A = false;
	bool useT_A = false;
	bool cmdSeal = false; //
	bool stateAux = false;
	bool globalState = true;
	bool globalStateAux = false;
	bool sPin = !command; // Output sate

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
};

#endif