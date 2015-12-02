#include "image.h"
using namespace std;

Pixel::Pixel()
{
	red = 0;
	green = 0;
	blue = 0;
}

Pixel::Pixel(double r, double g, double b)
{
	red = (unsigned int) (std::max(0.0, std::min(255.0, r)) + 0.5);
	green = (unsigned int) (std::max(0.0, std::min(255.0, g)) + 0.5);
	blue = (unsigned int) (std::max(0.0, std::min(255.0, b)) + 0.5);
}

void Image::init()
{
	pppPixels = new Pixel*[iWidth]();
	for (int y = 0; y < iHeight; y++)
	{
		pppPixels[y] = new Pixel[iHeight]();
	}
}

Image::~Image()
{
	for(int x = 0; x < iWidth; x++)
	{
		delete[] pppPixels[x];
	}
	delete[] pppPixels;
}

Image::Image(int size)
{
	iWidth = size;
	iHeight = size;
	init();
}

Image::Image(int width, int height)
{
	iWidth = width;
	iHeight = height;
	init();
}

Pixel Image::get(int width, int height)
{
	if(width < 0 || height < 0 || width >= iWidth || height >= iHeight)
	{
		throw std::out_of_range ("Index out of range.");
	}
	return pppPixels[width][height];
}

void Image::set(int width, int height, Pixel value)
{
	if(width < 0 || height < 0 || width >= iWidth || height >= iHeight)
	{
		throw std::out_of_range ("Index out of range.");
	}
	pppPixels[width][height] = value;
}

// BMP
void Image::save(const char* filename)
{
	static char buffer0[] = {
		0x00, 0x00, // Unused
		0x00, 0x00, // Unused
		0x36, 0x00, 0x00, 0x00, // Offset to pixel array (54)
		// DIB Header
		0x28, 0x00, 0x00, 0x00, // Size of DIB Header
	};
	static char buffer1[] = {
		0x01, 0x00, // Number of planes
		0x18, 0x00, // Number of bits per pixel (24)
		0x00, 0x00, 0x00, 0x00, // Compression method (none)
	};
	static char buffer2[] = {
		0x13, 0x0B, 0x00, 0x00, // Horizontal resolution
		0x13, 0x0B, 0x00, 0x00, // Vertical resolution
		0x00, 0x00, 0x00, 0x00, // Colors in pallete
		0x00, 0x00, 0x00, 0x00 // Important colors
	};
	FILE * file;
	file = fopen(filename, "w");
	if (file != NULL)
	{
		// 3 bytes per pixel, plus padding to align to four bytes
		int padding = iWidth % 4;
		int rowsize = iWidth * 3 + padding;
		int bitmapsize = rowsize * iHeight;
		// 14 is size of BMP Header, 40 is size of DIB Header
		int filesize = 14 + 40 + bitmapsize;

		// ID
		fputs("BM", file);
		// Filesize
		fwrite(&filesize, sizeof(int), 1, file);

		fwrite(buffer0, sizeof(char), sizeof(buffer0), file);
		fwrite(&iWidth, sizeof(int), 1, file); // Image width
		fwrite(&iHeight, sizeof(int), 1, file); // Image height
		fwrite(buffer1, sizeof(char), sizeof(buffer1), file);
		fwrite(&bitmapsize, sizeof(int), 1, file); // Size of bitmap data including padding
		fwrite(buffer2, sizeof(char), sizeof(buffer2), file);
		// Pixel data starts at bottom left and goes across each row working its way up
		for(int y = iHeight - 1; y >= 0; y--)
		{
			for(int x = 0; x < iWidth; x++)
			{
				Pixel p = pppPixels[x][y];
				// The data is ordered backwards from normal
				fputc(p.blue, file);
				fputc(p.green, file);
				fputc(p.red, file);
			}
			for(int i = 0; i < padding; i++)
			{
				// Pad the end of the row
				fputc(0, file);
			}
		}
	}
	fclose(file);
}
