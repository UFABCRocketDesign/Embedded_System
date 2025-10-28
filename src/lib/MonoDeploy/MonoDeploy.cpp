#include "MonoDeploy.h"

///Controle de acionamento unit�rio
bool MonoDeploy::apogee = false;
unsigned long MonoDeploy::TimeZero = 0;
unsigned long MonoDeploy::Tseal = 0;
float MonoDeploy::height = 0;
MonoDeploy::MonoDeploy(unsigned int commandPin, unsigned int infoPin, unsigned long actionTime, unsigned int systemMode): cPin(commandPin), iPin(infoPin), sysMode(systemMode), Tign((unsigned long)(actionTime * 1000000.0))
{
}

void MonoDeploy::resetTimer() // To reset timer on liftoff
{
	TimeZero = micros();
}

void MonoDeploy::sealApogee(bool apg)
{
	if (!apogee)
	{
		Tseal = micros() - TimeZero;
		apogee = apg;
	}
}

bool MonoDeploy::getApogee()
{
	return apogee;
}

void MonoDeploy::putHeight(float H)
{
	height = H;
}

float MonoDeploy::getDeploymentHeight()
{
	return deploymentHeight;
}

void MonoDeploy::setHeightCmd(float H)
{
	useH = true;
	cmdHeight = H;
}

void MonoDeploy::setDelayCmd(float T)
{
	useT = true;
	cmdDelay = (unsigned long)(T * 1000000.0f);
}


void MonoDeploy::setTmax(float T)
{
	useM = true;
	Tmax = (unsigned long)(T * 1000000.0f);
}

bool MonoDeploy::begin()
{
	if (sysMode == 0)
	{
		pinMode(cPin, OUTPUT);
		digitalWrite(cPin, sPin);
		pinMode(iPin, INPUT);
	}
#if SERVO_MODE
	else if(sysMode)
	{
		motor.attach(cPin);
		motor.write(0);
	}
#endif
	return info();
}

bool MonoDeploy::info()
{
	return analogRead(iPin) > 100;
}

bool MonoDeploy::getState(bool type)
{
	if (type) return sPin == command;
	else if (!stateAux && sPin == command)
	{
		stateAux = true;
		return true;
	}
	else return false;
}

bool MonoDeploy::getGlobalState()
{
	return globalState;
}

void MonoDeploy::refresh()
{
	Tnow = micros() - TimeZero;
	if (useM && Tnow >= TimeZero + Tmax && !getApogee()) sealApogee(true);
	if (getApogee())
	{
		if (useH && !useH_A) // If use Height and not reached yet
		{
			if (height <= cmdHeight) // height reached
			{
				useH_A = true;
				Theight = Tnow;
			}
		}
		if (useT && !useT_A) // If use Time and not reached yet
		{
			useT_A = (useH) ? ((Tnow >= Theight + cmdDelay) && useH_A) : (Tnow >= Tseal + cmdDelay);
		}
		bool triggers = (!useH && !useT) || // If dont use height nor delayed
			(useH && !useT && useH_A) || // If only use height
			(!useH && useT && useT_A) || // If only use delayed
			(useH && useT && useH_A && useT_A); // If use both
		if (cmdSeal || triggers)
		{
			if (!cmdSeal)
			{
				cmdSeal = true;
				Tcmd = Tnow;
				deploymentHeight = height;
			}
			if ((Tnow - Tcmd < Tign) && cmdSeal) sPin = command;
			else sPin = !command;
		}
	}
	if(sysMode == 0) digitalWrite(cPin, sPin);
#if SERVO_MODE
	else if(sysMode == 1)
	{
		if (sPin) motor.write(90);
		else motor.write(0);
	}
#endif
	if (!globalStateAux && sPin == command)	globalStateAux = true;
	else if (globalStateAux && sPin != command)	globalState = false;
}

void MonoDeploy::emergency(bool state, float T_EM)
{
	if(state && !emergencyState) // Begin of emergency case, saving states
	{
		useH_EM = useH; // Save original settings
		useT_EM = useT; // Save original settings
		cmdDelay_EM = cmdDelay; // Save original settings
		useH = false;
		useT = true; // Use delay mode to guarantee deployments
		cmdDelay = (unsigned long)(T_EM * 1000000.0f);
		// sealApogee(true); // It is not necessary, since another section will ensure this
	}
	if(!state && emergencyState) // End of emergency case, resumption of states
	{
		useH = useH_EM; // Restore original settings
		useT = useT_EM; // Restore original settings
		cmdDelay = cmdDelay_EM;
	}
	emergencyState = state;
}