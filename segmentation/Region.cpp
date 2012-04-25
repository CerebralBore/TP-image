#include "Region.h"

using namespace std;

Region::Region(void)
{
}

Region::Region(int a, int b)
{
	alive = true;
	coulTotR = 0;
	coulTotV = 0;
	coulTotB = 0;
	nbPixel = 0;
	frontiere.resize(0);
	frontiere.push_back(Pixel(a, b));
}

Region::~Region(void)
{
}

int Region::size()
{
	return frontiere.size();
}

int Region::getPixelX(int k)
{
	return frontiere[k].x();
}

int Region::getPixelY(int k)
{
	return frontiere[k].y();
}

int Region::nbPix()
{
	return nbPixel;
}

void Region::addPix(double a, double b, double c)
{
	coulTotR += a;
	coulTotV += b;
	coulTotB += c;
	nbPixel += 1;
}

void Region::setFrontiere(vector<Pixel> f)
{
	frontiere.clear();
	frontiere.resize(0);
	frontiere = f;
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

bool Region::compare(double r, double b, double v, double seuil)
{
	if( r > moyR()-seuil && r < moyR()+seuil)
	{
		if( b > moyB()-seuil && b < moyB()+seuil)
		{
			if( v > moyV()-seuil && v < moyV()+seuil)
			{
				return true;
			}
			else { return false; }
		}
		else { return false; }
	}
	else { return false; }
}

Pixel Region::getPixel(int a)
{
	return frontiere[a];
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