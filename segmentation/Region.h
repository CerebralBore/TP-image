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
	int cadreH;
	int cadreG;
	int cadreB;
	int cadreD;
	std::vector<Pixel> frontiere;

public:
	Region(void);
	Region(int x, int y, int rows, int cols);
	~Region(void);
	int sizeFr();
	int getFrPixelX(int p);
	int getFrPixelY(int p);
	Pixel getFrPixel(int pixel);
	void insertFrPixel(Pixel pixel);
	int nbPix();
	void addPix(double r, double v, double b);
	void setFrontiere(std::vector<Pixel> pixels);
	double moyR();
	double moyV();
	double moyB();
	bool compare(double r, double v, double b, double seuil);
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