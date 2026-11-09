// TriA.h

#pragma once

#ifndef _TRIA_h
#define _TRIA_h

class TriA
{
protected:
	float X = 0;
	float Y = 0;
	float Z = 0;

public:
	virtual bool readAll() = 0;
	float getX();
	float getY();
	float getZ();
};

#endif