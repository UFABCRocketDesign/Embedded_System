#include "VirtualGPS.h"

VirtualGPS::VirtualGPS(VirtualBridge &bridge, float recalT) :
GNSS(0, recalT),
br(bridge),
hLat(-1),
hLon(-1),
hAlt(-1),
hSat(-1),
hPrec(-1),
hChars(-1),
hAge(-1),
hKph(-1),
hMps(-1),
hYear(-1),
hMonth(-1),
hDay(-1),
hHour(-1),
hMinute(-1),
hSecond(-1),
lastSeq(0)
{}

void VirtualGPS::begin()
{
	hLat = br.handle("La");
	hLon = br.handle("Lo");
	hAlt = br.handle("Ll");
	hSat = br.handle("Lt");
	hPrec = br.handle("Lp");
	hChars = br.handle("Lc");
	hAge = br.handle("Lg");
	hKph = br.handle("Lk");
	hMps = br.handle("Lf");
	hYear = br.handle("Ly");
	hMonth = br.handle("Lm");
	hDay = br.handle("Ld");
	hHour = br.handle("Lh");
	hMinute = br.handle("Ln");
	hSecond = br.handle("Ls");
}

bool VirtualGPS::readAll()
{
	thisReadT = micros();

	const uint16_t s = br.seq(hLat);
	state = (s != 0) && (s != lastSeq);	// so conta como leitura boa se veio dado novo

	if (state) {
		if (getTimeLapse() > recalibrateT)
		{
			// begin();
#if PRINT
			Serial.println(F("Trigger Relac L"));
#endif // PRINT
		}

		lastSeq   = s;
		latitude = br.value(hLat);
		longitude = br.value(hLon);
		altitude = br.value(hAlt);
		satellites = (unsigned char) br.value(hSat);
		precision = (unsigned long) br.value(hPrec);
		chars = (unsigned long) br.value(hChars);
		age = (unsigned long) br.value(hAge);
		Kph = br.value(hKph);
		mps = br.value(hMps);
		year = (int) br.value(hYear);
		month = (byte) br.value(hMonth);
		day = (byte) br.value(hDay);
		hour = (byte) br.value(hHour);
		minute = (byte) br.value(hMinute);
		second = (byte) br.value(hSecond);

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	return state;
}
