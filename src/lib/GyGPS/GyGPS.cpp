#include "GyGPS.h"

///Dados do GPS
GyGPS::GyGPS(HardwareSerial &S, short gmt) :GpSerial(S), GMT(gmt), Sens(NULL)
{
}

unsigned short GyGPS::getFailed()
{
	return failed;
}

unsigned short GyGPS::getSentences()
{
	return sentences;
}

unsigned char GyGPS::getSatellites()
{
	return satellites;
}

unsigned long GyGPS::getPrecision()
{
	return precision;
}

unsigned long GyGPS::getChars()
{
	return chars;
}

unsigned long GyGPS::getAge()
{
	return age;
}

float GyGPS::getLatitude()
{
	return latitude;
}

float GyGPS::getLongitude()
{
	return longitude;
}

float GyGPS::getAltitude()
{
	return altitude;
}

float GyGPS::getKph()
{
	return Kph;
}

float GyGPS::getMps()
{
	return mps;
}

int GyGPS::getYear()
{
	return year;
}

byte GyGPS::getMonth()
{
	return month;
}

byte GyGPS::getDay()
{
	return day;
}

byte GyGPS::getHour(bool gmt)
{
	return hour + gmt ? GMT : 0;
}

byte GyGPS::getMinute()
{
	return minute;
}

byte GyGPS::getSecond()
{
	return second;
}

void GyGPS::begin()
{
	GpSerial.begin(9600);
}

bool GyGPS::readAll()
{
	thisReadT = micros();
	state = false;
	if (GpSerial.available()) while (GpSerial.available())
	{
		char c = GpSerial.read();
		state = gps.encode(c);
	}
	if (state)
	{
		gps.f_get_position(&latitude, &longitude, &age);
		satellites = gps.satellites();
		precision = gps.hdop();
		altitude = gps.f_altitude();
		Kph = gps.f_speed_kmph();
		mps = gps.f_speed_mps();

		lastWorkT = thisReadT;
	}
	lastReadT = thisReadT;
	gps.crack_datetime(&year, &month, &day, &hour, &minute, &second);
	gps.stats(&chars, &sentences, &failed);
	return state;
}

bool GyGPS::isNew()
{
	return state;
}