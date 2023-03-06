#include "fsdf_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <assert.h>

Pixel Image::getPixel(uint32_t x, uint32_t y)
{
	uint32_t rowOffset = y * _width;
	auto rowBegin = data + rowOffset;

	return rowBegin[x];
}

void Image::setExtent(uint32_t width, uint32_t height)
{
	_width = width;
	_height = height;
	_values.resize(_width * static_cast<size_t>(_height));
	data = _values.data();
}

void Image::setPixel(uint32_t x, uint32_t y, Pixel& value)
{
	size_t rowOffset = static_cast<size_t>(y) * _width;

	for (int i = 0; i < 4; ++i)
	{
		_values[rowOffset + x][i] = value[i];
	}
}

void Image::readFile(const char* imgPath)
{
	int tmpWidth = 0;
	int tmpHeight = 0;
	auto tmpData = reinterpret_cast<Pixel*>(stbi_load(imgPath, &tmpWidth, &tmpHeight, &_channels, 4));

	assert(tmpData);
	assert(tmpWidth);
	assert(tmpHeight);

	data = tmpData;
	_width = tmpWidth;
	_height = tmpHeight;
}

void Image::writeToFile(const char* imgPath)
{
	assert(data);
	assert(_width);
	assert(_height);

	int result = 0;

	result = stbi_write_bmp(imgPath, _width, _height, _channels, data);

	assert(result);
}