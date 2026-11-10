// TriA.hpp

#pragma once

#ifndef _TRIA_HPP
#define _TRIA_HPP

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