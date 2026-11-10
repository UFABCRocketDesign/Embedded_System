// RoundArray.hpp

#pragma once

#ifndef _ROUNDARRAY_HPP
#define _ROUNDARRAY_HPP

template <typename type>
class RoundArray
{
	const size_t Len;
	type *Vals = new type[Len]();
	size_t pointer = 0;

public:
	RoundArray(const unsigned int &len);
	~RoundArray(); // Destructor
	void push(const type &var);
	operator float();
	type &operator[](int index);
	void operator=(const type &var);
	size_t length();
};

#include "RoundArray.tpp"

#endif