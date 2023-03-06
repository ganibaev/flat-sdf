#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

struct Pixel
{
	unsigned char r = 255;
	unsigned char g = 255;
	unsigned char b = 255;
	unsigned char a = 255;

	unsigned char& operator[](int i)
	{
		if (i > 3 || i < 0)
		{
			throw std::runtime_error("RGBA index must be in the interval [0, 3]");
		}
		switch (i)
		{
		case 0:
			return r;
			break;
		case 1:
			return g;
			break;
		case 2:
			return b;
			break;
		case 3:
			return a;
			break;
		default:
			break;
		}
	}
};

class Image
{
public:

	uint32_t _width = 0;
	uint32_t _height = 0;
	int _channels = 4;
	
	Pixel getPixel(uint32_t x, uint32_t y);
	void setExtent(uint32_t width, uint32_t height);
	void setPixel(uint32_t x, uint32_t y, Pixel& value);
	void readFile(const char* imgPath);
	void writeToFile(const char* imgPath);

	std::vector<Pixel> _values;
	Pixel* data = _values.data();
};
