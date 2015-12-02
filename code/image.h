#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <iostream>
#include <fstream>
#include <stdexcept>

struct Pixel {
	unsigned int red;
	unsigned int green;
	unsigned int blue;
	Pixel();
	Pixel(double, double, double);
	void cull(double, double);
};

class Image {
private:
	unsigned int iWidth;
	unsigned int iHeight;
	Pixel ** pppPixels;
	void init();

public:
	~Image();
	Image(int);
	Image(int, int);
	Pixel get(int, int);
	void set(int, int, Pixel);
	void save(const char*);
};

#endif
