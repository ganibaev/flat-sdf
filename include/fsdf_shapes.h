#pragma once
#include <cstdint>
#include <array>
#include "fsdf_image.h"
#include <algorithm>
#include <cmath>
#include <iostream>

inline float length(float x, float y)
{
	return std::sqrt(x * x + y * y);
}

inline float lerp(float s1, float s2, float x1, float x2, float x)
{
	if (x1 == x2)
	{
		return s1;
	}
	return (x - x1) / (x2 - x1) * s2 + (x2 - x) / (x2 - x1) * s1;
}

inline float blerp(float s11, float s12, float s21, float s22, float x1, float x2, float y1, float y2, float x, float y)
{
	if (x1 == x2)
	{
		return lerp(s21, s22, y1, y2, y);
	}
	if (y1 == y2)
	{
		return lerp(s12, s22, x1, x2, x);
	}
	float x2x1, y2y1, x2x, y2y, yy1, xx1;
	x2x1 = x2 - x1;
	y2y1 = y2 - y1;
	x2x = x2 - x;
	y2y = y2 - y;
	yy1 = y - y1;
	xx1 = x - x1;
	float res = (1.0 / (x2x1 * y2y1)) * (
		s11 * x2x * y2y +
		s21 * xx1 * y2y +
		s12 * x2x * yy1 +
		s22 * xx1 * yy1
		);
	return res;
}

template <typename T>
int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

class Shape {
public:
	Shape(int xPosition, int yPosition, Pixel color, char sType, float scale = 1.0f, bool outline = false);
	virtual unsigned char getSDF(int x, int y);
	constexpr static float spread = 2.0f;

	Pixel _color;
	char _type;
	float _scale;
	bool _outline;

	int _xPosition;
	int _yPosition;
};

class Circle : public Shape
{
public:
	Circle(int xPosition, int yPosition, Pixel color, float radius, bool outline = false);
	static unsigned char calculateSDF(int x, int y);
	static constexpr std::array<unsigned char, 256 * 256> fillValues();
	unsigned char getSDF(int x, int y) override;

private:
	static std::array<unsigned char, 256 * 256> _sdfValues;
	float _radius = 0.0;
};

class Rectangle : public Shape
{
public:
	Rectangle(int xPosition, int yPosition, Pixel color, float height, bool outline = false);
	static unsigned char calculateSDF(int x, int y);
	static constexpr std::array<unsigned char, 256 * 256> fillValues();
	unsigned char getSDF(int x, int y) override;

private:
	static std::array<unsigned char, 256 * 256> _sdfValues;
	float _height = 0.0;
};

class Triangle : public Shape
{
public:
	Triangle(int xPosition, int yPosition, Pixel color, float radius, bool outline = false);
	static unsigned char calculateSDF(int x, int y);
	static constexpr std::array<unsigned char, 256 * 256> fillValues();
	unsigned char getSDF(int x, int y) override;

private:
	static std::array<unsigned char, 256 * 256> _sdfValues;
	float _radius = 0.0;
};

class Line : public Shape
{
public:
	Line(int xPosition, int yPosition, Pixel color, float width);
	static unsigned char calculateSDF(int x, int y);
	static constexpr std::array<unsigned char, 256 * 256> fillValues();
	unsigned char getSDF(int x, int y) override;

private:
	static std::array<unsigned char, 256 * 256> _sdfValues;
	float _width = 0.0;
};

class Pic : public Shape
{
public:
	Pic(int xPosition, int yPosition, Pixel color, float height, std::string filePath, Pixel topColor, bool outline = false);
	void fillValues();
	unsigned char getSDF(int x, int y) override;
	Pixel _topColor;
	std::string _filePath;

private:
	std::array<unsigned char, 256 * 256> _sdfValues;
	float _height = 0.0;
};