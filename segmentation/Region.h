#ifndef __REGION
#define __REGION

#include <vector>
#include "Pixel.h"

class Region
{
private:
	bool alive;
	double coulTotR;
	double coulTotV;
	double coulTotB;
	int nbPixel;
	std::vector<Pixel> frontiere;

public:
	Region(void);
	Region(int, int);
	~Region(void);
	int size();
	int getPixelX(int);
	int getPixelY(int);
	int nbPix();
	void addPix(double, double, double);
	void setFrontiere(std::vector<Pixel>);
	double moyR();
	double moyV();
	double moyB();
	bool compare(double, double, double, double);
	Pixel getPixel(int);
	void mmmmmmmonsterKill();
	bool isAlive();

};

#endif