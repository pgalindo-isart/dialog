#pragma once

#include <cstdint>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef unsigned char u8;

static float f32_min(float a, float b)
{
	return a < b ? a : b;
}

static float f32_max(float a, float b)
{
	return a > b ? a : b;
}

struct vec2_t
{
	float x;
	float y;
};

struct irect_t
{
	int x;
	int y;
	int w;
	int h;
};

struct rect_t
{
	float x;
	float y;
	float w;
	float h;

	bool contains(vec2_t pos) const
	{
		return   pos.x >= this->x && pos.x < this->x + this->w
			  && pos.y >= this->y && pos.y < this->y + this->h;
	}

	bool intersects(rect_t other) const
	{
		return f32_max(this->x, other.x) < f32_min(this->x + this->w, other.x + other.w) &&
			   f32_max(this->y, other.y) < f32_min(this->y + this->h, other.y + other.h);
	}
};

struct color_t
{
	color_t() : value(0x000000ff) {}
	color_t(uint32_t value) : value(value) {}

	union
	{
		uint32_t value;
		u8 c[4];
		struct {
			u8 a;
			u8 b;
			u8 g;
			u8 r;
		};
	};
};