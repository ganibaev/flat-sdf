#include "fsdf_shapes.h"

struct Point
{
	float x = 0.0f;
	float y = 0.0f;
};

Shape::Shape(int xPosition, int yPosition, Pixel color, char sType, float scale, bool outline)
	: _xPosition(xPosition), _yPosition(yPosition), _color(color), _type(sType), _scale(scale), _outline(outline) {};

unsigned char Shape::getSDF(int x, int y)
{
	return 0;
}

Circle::Circle(int xPosition, int yPosition, Pixel color, float radius, bool outline)
	: Shape(xPosition, yPosition, color, 'c', 64.0f / radius, outline), _radius(radius) {};

unsigned char Circle::calculateSDF(int x, int y)
{
	float xf = static_cast<float>(x) - 127.0f;
	float yf = static_cast<float>(y) - 127.0f;
	float dist = length(xf, yf) - 64.0f + Shape::spread / 2;
	float clamped = 0.0f;
	if (dist >= Shape::spread)
	{
		clamped = 1.0f;
	}
	else if (dist <= 0)
	{
		clamped = 0.0f;
	}
	else
	{
		clamped = dist / Shape::spread;
	}
	return static_cast<unsigned char>(clamped * 255.0f);
}

constexpr std::array<unsigned char, 256 * 256> Circle::fillValues()
{
	std::array<unsigned char, 256 * 256> sdfValues = { 0 };
	for (int x = 0; x < 256; ++x)
	{
		for (int y = 0; y < 256; ++y)
		{
			sdfValues[256 * static_cast<size_t>(y) + x] = calculateSDF(x, y);
		}
	}
	return sdfValues;
}

std::array<unsigned char, 256 * 256> Circle::_sdfValues = fillValues();

unsigned char Circle::getSDF(int x, int y)
{
	return _sdfValues[256 * static_cast<size_t>(y) + x];
}

Rectangle::Rectangle(int xPosition, int yPosition, Pixel color, float height, bool outline)
	: Shape(xPosition, yPosition, color, 'r', 48.0f / height, outline), _height(height) {};

unsigned char Rectangle::calculateSDF(int x, int y)
{
	float xf = static_cast<float>(x) - 127.0f;
	float yf = static_cast<float>(y) - 127.0f;
	Point d;
	d.x = std::abs(xf) - 96.0f;
	d.y = std::abs(yf) - 48.0f;
	float dist = length(std::max(d.x, 0.0f), std::max(d.y, 0.0f)) + std::min(std::max(d.x, d.y), 0.0f) + Shape::spread / 2;

	float clamped = 0.0f;
	if (dist >= Shape::spread)
	{
		clamped = 1.0f;
	}
	else if (dist <= 0)
	{
		clamped = 0.0f;
	}
	else
	{
		clamped = dist / Shape::spread;
	}
	return static_cast<unsigned char>(clamped * 255.0f);
}

constexpr std::array<unsigned char, 256 * 256> Rectangle::fillValues()
{
	std::array<unsigned char, 256 * 256> sdfValues = { 0 };
	for (int x = 0; x < 256; ++x)
	{
		for (int y = 0; y < 256; ++y)
		{
			auto sdf = calculateSDF(x, y);
			sdfValues[256 * static_cast<size_t>(y) + x] = sdf;
		}
	}
	return sdfValues;
}

std::array<unsigned char, 256 * 256> Rectangle::_sdfValues = fillValues();

unsigned char Rectangle::getSDF(int x, int y)
{
	return _sdfValues[256 * static_cast<size_t>(y) + x];
}



Triangle::Triangle(int xPosition, int yPosition, Pixel color, float radius, bool outline)
	: Shape(xPosition, yPosition, color, 't', 64.0f / radius, outline), _radius(radius) {};

unsigned char Triangle::calculateSDF(int x, int y)
{
	float xf = static_cast<float>(x) - 127.0f;
	float yf = static_cast<float>(y) - 127.0f;
	
	const float r = 64.0f;
	const float k = std::sqrt(3.0f);
	Point p;
	p.x = abs(xf) - r;
	p.y = yf + r / k;
	if (p.x + k * p.y > 0.0f)
	{
		auto oldx = p.x;
		p.x = (p.x - k * p.y) / 2.0f;
		p.y = (-k * oldx - p.y) / 2.0f;
	}
	p.x -= std::clamp(p.x, -2.0f * r, 0.0f);

	float dist = -length(p.x, p.y) * sgn(p.y) + Shape::spread / 2;

	float clamped = 0.0f;
	if (dist >= Shape::spread)
	{
		clamped = 1.0f;
	}
	else if (dist <= 0)
	{
		clamped = 0.0f;
	}
	else
	{
		clamped = dist / Shape::spread;
	}
	return static_cast<unsigned char>(clamped * 255.0f);
}

constexpr std::array<unsigned char, 256 * 256> Triangle::fillValues()
{
	std::array<unsigned char, 256 * 256> sdfValues = { 0 };
	for (int x = 0; x < 256; ++x)
	{
		for (int y = 0; y < 256; ++y)
		{
			auto sdf = calculateSDF(x, y);
			sdfValues[256 * static_cast<size_t>(y) + x] = sdf;
		}
	}
	return sdfValues;
}

std::array<unsigned char, 256 * 256> Triangle::_sdfValues = fillValues();

unsigned char Triangle::getSDF(int x, int y)
{
	return _sdfValues[256 * static_cast<size_t>(y) + x];
}



Line::Line(int xPosition, int yPosition, Pixel color, float width)
	: Shape(xPosition, yPosition, color, 'l', 128.0f / width), _width(width) {};

unsigned char Line::calculateSDF(int x, int y)
{
	float xf = static_cast<float>(x);
	float yf = static_cast<float>(y);

	Point a{ 64.0f, 128.0f }, b{ 192.0f, 128.0f };

	Point pa, ba;
	pa.x = xf - a.x;
	pa.y = yf - a.y;
	
	ba.x = b.x - a.x;
	ba.y = b.y - a.y;

	float h = std::clamp((pa.x * ba.x + pa.y * ba.y) / (ba.x * ba.x + ba.y * ba.y), 0.0f, 1.0f);

	float dist = length(pa.x - ba.x * h, pa.y - ba.y * h) + Shape::spread / 2;

	float clamped = 0.0f;
	if (dist >= Shape::spread)
	{
		clamped = 1.0f;
	}
	else if (dist <= 0)
	{
		clamped = 0.0f;
	}
	else
	{
		clamped = dist / Shape::spread;
	}
	return static_cast<unsigned char>(clamped * 255.0f);
}

constexpr std::array<unsigned char, 256 * 256> Line::fillValues()
{
	std::array<unsigned char, 256 * 256> sdfValues = { 0 };
	for (int x = 0; x < 256; ++x)
	{
		for (int y = 0; y < 256; ++y)
		{
			auto sdf = calculateSDF(x, y);
			sdfValues[256 * static_cast<size_t>(y) + x] = sdf;
		}
	}
	return sdfValues;
}

std::array<unsigned char, 256 * 256> Line::_sdfValues = fillValues();

unsigned char Line::getSDF(int x, int y)
{
	return _sdfValues[256 * static_cast<size_t>(y) + x];
}



Pic::Pic(int xPosition, int yPosition, Pixel color, float height, std::string filePath, Pixel topColor, bool outline)
	: Shape(xPosition, yPosition, color, 'p', 256.0f / height, outline), _height(height), _filePath(filePath), _topColor(topColor)
{
	fillValues();
};


void Pic::fillValues()
{
	Image sdfs;
	sdfs.readFile(_filePath.c_str());

	for (int x = 0; x < sdfs._width; ++x)
	{
		for (int y = 0; y < sdfs._height; ++y)
		{
			_sdfValues[sdfs._width * static_cast<size_t>(y) + x] = 255 - sdfs.data[sdfs._width * static_cast<size_t>(y) + x].g;
		}
	}
}

unsigned char Pic::getSDF(int x, int y)
{
	return _sdfValues[256 * static_cast<size_t>(y) + x];
}