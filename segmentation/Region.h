#ifndef __REGION
#define __REGION

#include <vector>
#include "Pixel.h"

class Region
{
private:
	bool alive;
	double coulInitR;
	double coulInitV;
	double coulInitB;
	double coulTotR;
	double coulTotV;
	double coulTotB;
	int nbPixel;
	int cadreH;
	int cadreG;
	int cadreB;
	int cadreD;
	std::vector<Pixel> frontiere;

public:
	Region(void);
	Region(int x, int y, int rows, int cols);
	~Region(void);
	int size();
	int getPixelX(int p);
	int getPixelY(int p);
	int nbPix();
	void addPix(double r, double v, double b);
	void setFrontiere(std::vector<Pixel> pixels);
	double moyR();
	double moyV();
	double moyB();
	bool compare(double r, double v, double b, double seuil);
	Pixel getPixel(int pixel);
	void pixelCadre(int x, int y);
	void regionCadre(int xH, int yG, int xB, int yD);
	int getCadreH();
	int getCadreG();
	int getCadreB();
	int getCadreD();
	void mmmmmmmonsterKill();
	bool isAlive();

};

#endif