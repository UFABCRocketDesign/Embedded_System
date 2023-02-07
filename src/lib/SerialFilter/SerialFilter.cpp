#include "SerialFilter.h"

SerialFilter::SerialFilter(int n, int v) : N(n)
{
	for (int i = 0; i < N; i++) MM[i] = new MovingAverage(v);
}

SerialFilter::SerialFilter(int n, int V[]) : N(n)
{
	for (int i = 0; i < N; i++) MM[i] = new MovingAverage(V[i]);
}

float SerialFilter::addValor(const float & valor)
{
	(*MM[N - 1]) = valor;
	for (int i = N - 1; i > 0; i--) (*MM[i - 1]) = ((*MM[i]).getMedia());
	return (*MM[0]).getMedia();
}

float SerialFilter::getMedia()
{
	return(*MM[0]);
}

float SerialFilter::operator=(const float &valor)
{
	return addValor(valor);
}

SerialFilter::operator float()
{
	return (*MM[0]);
}