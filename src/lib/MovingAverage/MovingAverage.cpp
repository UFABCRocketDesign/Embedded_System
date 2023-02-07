#include "MovingAverage.h"

MovingAverage::MovingAverage(int n) : N(n), Vals(n)
{
}

float MovingAverage::addValor(const float & valor)
{
	media *= N;
	media -= Vals;
	media += valor;
	media /= N;
	Vals = valor;
	max = (media > max) ? media : max;
	min = (media < min) ? media : min;
	return media;
}

float MovingAverage::getMedia()
{
	return media;
}

float MovingAverage::getVar()
{
	sigma = 0;
	for (unsigned int i = 0; i < N; i++) sigma += pow(Vals[i] - media, 2.f);
	sigma = pow(sigma / (N - 1), .5f);
	return sigma;
}

float MovingAverage::getMax()
{
	return max;
}

float MovingAverage::getMin()
{
	return min;
}

float MovingAverage::operator=(const float & valor)
{
	return addValor(valor);
}

MovingAverage::operator float()
{
	return media;
}