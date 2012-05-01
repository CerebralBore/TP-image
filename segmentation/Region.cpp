#include "Region.h"

using namespace std;

Region::Region(void)
{
}

Region::Region(int x, int y, int rows, int cols)
{
	alive = true;
	coulInitR = -1;
	coulInitV = -1;
	coulInitB = -1;
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

int Region::size()
{
	return frontiere.size();
}

int Region::getPixelX(int p)
{
	return frontiere[p].x();
}

int Region::getPixelY(int p)
{
	return frontiere[p].y();
}

int Region::nbPix()
{
	return nbPixel;
}

void Region::addPix(double r, double v, double b)
{
	/*if(coulInitR == -1)
	{
		coulInitR = r;
		coulInitV = v;
		coulInitB = b;
	}*/
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
	/*if( r > coulInitR-seuil && r < coulInitR+seuil)
	{
		if( v > coulInitV-seuil && v < coulInitV+seuil)
		{
			if( b > coulInitB-seuil && b < coulInitB+seuil)*/
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

Pixel Region::getPixel(int pixel)
{
	return frontiere[pixel];
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