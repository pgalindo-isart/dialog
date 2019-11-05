#pragma once

#include <cmath>

static float f32_round(float x)
{
	return std::round(x);
}

static float f32_sin(float t)
{
	return std::sin(t);
}

static float f32_cos(float t)
{
	return std::cos(t);
}

static float f32_lerp(float x, float y, float a)
{
	return (1 - a) * x + a * y;
}

static float f32_invLerp(float edge0, float edge1, float x)
{
	return (x - edge0) / (edge1 - edge0);
}

static float f32_clamp(float x, float min, float max)
{
	return (x <= min) ? min : (x >= max) ? max : x;
}

static float f32_step(float edge, float x)
{
	return (x < edge) ? 0.f : 1.f;
}

static float f32_smoothStep(float edge0, float edge1, float x)
{
	float t = f32_clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return t * t * (3.f - 2.f * t);
}

static float f32_linearRemap(float inMin, float inMax, float v, float outMin, float outMax, bool clamp)
{
	float result = outMin + (v - inMin) * (outMax - outMin) / (inMax - inMin);
	return clamp ? f32_clamp(result, outMin, outMax) : result;
}

// android/view/animation/
static float tween_overshoot(float t, float tension = 2.f)
{
	t -= 1.f;
	return t * t * ((tension + 1.f) * t + tension) + 1.f;
}
