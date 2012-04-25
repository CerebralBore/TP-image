#include "Pixel.h"

Pixel::Pixel(void)
{
}

Pixel::Pixel(int a, int b): px(a), py(b)
{
}

Pixel::~Pixel(void)
{
}

void Pixel::setX(int a)
{
	px = a;
}

void Pixel::setY(int a)
{
	py = a;
}

int Pixel::x()
{
	return px;
}
int Pixel::y()
{
	return py;
}
