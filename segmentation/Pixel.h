#ifndef __PIXEL
#define __PIXEL

class Pixel
{
private:
	int px;
	int py;
public:
	Pixel();
	Pixel(int, int);
	~Pixel(void);
	int x();
	int y();
	void setX(int);
	void setY(int);

};

#endif