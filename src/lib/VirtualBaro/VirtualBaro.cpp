#include "VirtualBaro.h"

///Barometro
VirtualBaro::VirtualBaro(VirtualBridge &bridge, float recalT) : Baro(0, recalT), br(bridge), hP(-1), hT(-1), lastSeq(0)
{
}

void VirtualBaro::begin()
{
	hP = br.handle("Bp");
	hT = br.handle("Bt");
}

bool VirtualBaro::readAll()
{
	thisReadT = micros();

	const uint16_t s = br.seq(hP);
	state = (s != 0) && (s != lastSeq);	// so conta como leitura boa se veio dado novo

	if (state) {
		if (getTimeLapse() > recalibrateT)
		{
			// begin();
#if PRINT
			Serial.println(F("Relacibrado B"));
#endif // PRINT
		}

		lastSeq   = s;
		pascal    = br.value(hP);
		celcius   = br.value(hT);

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}
