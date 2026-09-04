#include "VirtualAccel.h"

///Aceleometro
VirtualAccel::VirtualAccel(VirtualBridge &bridge, float recalT) : Accel(0, recalT), br(bridge), hX(-1), hY(-1), hZ(-1), lastSeq(0)
{}

void VirtualAccel::begin()
{
	hX = br.handle("Ax");
	hY = br.handle("Ay");
	hZ = br.handle("Az");
}

bool VirtualAccel::readAll()
{
	thisReadT = micros();

	const uint16_t s = br.seq(hX);
	state = (s != 0) && (s != lastSeq);	// so conta como leitura boa se veio dado novo

	if (state) {
		if (getTimeLapse() > recalibrateT)
		{
			// begin();
#if PRINT
			Serial.println(F("Trigger Recal A"));
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
