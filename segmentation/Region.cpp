#include "Region.h"

using namespace std;

Region::Region(void)
{
}

Region::Region(int x, int y, int rows, int cols)
{
	alive = true;
	coulTotR = 0;
	coulTotV = 0;
	coulTotB = 0;
	nbPixel = 0;
	cadreH = rows;
	cadreG = cols;
	cadreB = 0;
	cadreD = 0;
	frontiere.resize(0);
	frontiere.push_back(Pixel(x, y));
}

Region::~Region(void)
{
}

int Region::sizeFr()
{
	return frontiere.size();
}

int Region::getFrPixelX(int p)
{
	return frontiere[p].x();
}

int Region::getFrPixelY(int p)
{
	return frontiere[p].y();
}

Pixel Region::getFrPixel(int pixel)
{
	return frontiere[pixel];
}

void Region::insertFrPixel(Pixel pixel)
{
	frontiere.push_back(pixel);
}

int Region::nbPix()
{
	return nbPixel;
}

void Region::addPix(double r, double v, double b)
{
	coulTotR += r;
	coulTotV += v;
	coulTotB += b;
	nbPixel += 1;
}

void Region::setFrontiere(vector<Pixel> pixels)
{
	frontiere.clear();
	frontiere.resize(0);
	frontiere = pixels;
}

double Region::moyR()
{
	return coulTotR/nbPixel;
}

double Region::moyV()
{
	return coulTotV/nbPixel;
}

double Region::moyB()
{
	return coulTotB/nbPixel;
}

bool Region::compare(double r, double v, double b, double seuil)
{
	if( r > moyR()-seuil && r < moyR()+seuil)
	{
		if( v > moyV()-seuil && v < moyV()+seuil)
		{
			if( b > moyB()-seuil && b < moyB()+seuil)
			{
				return true;
			}
			else { return false; }
		}
		else { return false; }
	}
	else { return false; }
}

void Region::pixelCadre(int x, int y)
{
	if(x < cadreH)
	{
		cadreH = x;
	}
	if(y < cadreG)
	{
		cadreG = y;
	}
	if(x > cadreB)
	{
		cadreB = x;
	}
	if(y > cadreD)
	{
		cadreD = y;
	}
}

void Region::regionCadre(int xH, int yG, int xB, int yD)
{
	if(xH < cadreH)
	{
		cadreH = xH;
	}
	if(yG < cadreG)
	{
		cadreG = yG;
	}
	if(xB > cadreB)
	{
		cadreB = xB;
	}
	if(yD > cadreD)
	{
		cadreD = yD;
	}
}

int Region::getCadreH()
{
	return cadreH;
}

int Region::getCadreG()
{
	return cadreG;
}

int Region::getCadreB()
{
	return cadreB;
}

int Region::getCadreD()
{
	return cadreD;
}

void Region::mmmmmmmonsterKill()
{
	frontiere.clear();
	frontiere.resize(0);
	alive = false;
}

bool Region::isAlive()
{
	return alive;
}