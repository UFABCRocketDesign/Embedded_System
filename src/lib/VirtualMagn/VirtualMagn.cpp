#include "VirtualMagn.h"

///Aceleometro
VirtualMagn::VirtualMagn(VirtualBridge &bridge, float recalT) : Magn(0, recalT), br(bridge), hX(-1), hY(-1), hZ(-1), lastSeq(0)
{}

void VirtualMagn::begin()
{
	hX = br.handle("Mx");
	hY = br.handle("My");
	hZ = br.handle("Mz");
}

bool VirtualMagn::readAll()
{
	thisReadT = micros();

	const uint16_t s = br.seq(hX);
	state = (s != 0) && (s != lastSeq);	// so conta como leitura boa se veio dado novo

	if (state) {
		if (getTimeLapse() > recalibrateT)
		{
			// begin();
#if PRINT
			Serial.println(F("Trigger Recal M"));
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
