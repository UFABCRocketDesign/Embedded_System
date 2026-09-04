#include "VirtualGyro.h"

///Giroscopio
VirtualGyro::VirtualGyro(VirtualBridge &bridge, float recalT) : Gyro(-1, 0, recalT), br(bridge), hX(-1), hY(-1), hZ(-1), lastSeq(0)
{}

void VirtualGyro::begin()
{
	hX = br.handle("Gx");
	hY = br.handle("Gy");
	hZ = br.handle("Gz");
}

bool VirtualGyro::readAll()
{
	thisReadT = micros();

	const uint16_t s = br.seq(hX);
	state = (s != 0) && (s != lastSeq);	// so conta como leitura boa se veio dado novo

	if (state) {
		if (getTimeLapse() > recalibrateT)
		{
			// begin();
#if PRINT
			Serial.println(F("Trigger Recal G"));
#endif // PRINT
		}

		lastSeq   = s;
		X = br.value(hX);
		Y = br.value(hY);
		Z = br.value(hZ);

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}
