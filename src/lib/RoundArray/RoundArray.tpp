template <typename type> RoundArray<type>::RoundArray(const unsigned int &len) :Len(len)
{
}

template <typename type> RoundArray<type>::~RoundArray() // Destructor
{
	delete[] Vals;
}

template <typename type> void RoundArray<type>::push(const type &var)
{
	Vals[pointer] = var;
	pointer = pointer + 1 == Len ? 0 : pointer + 1;
}

template <typename type> RoundArray<type>::operator float()
{
	return Vals[pointer];
}

template <typename type> type &RoundArray<type>::operator[](int index)
{
	return Vals[(Len + pointer - index - 1) % Len];
}

template <typename type> void RoundArray<type>::operator=(const type &var)
{
	push(var);
}

template <typename type> size_t RoundArray<type>::length()
{
	return Len;
}