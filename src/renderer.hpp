#pragma once

#include <vector>

#include "types.h"

typedef int TextureId;

class Renderer
{
public:
	virtual ~Renderer() = default;
	virtual TextureId CreateTexture(int width, int height, unsigned char* pixels) = 0;
	
	virtual void RenderLine(float x0, float y0, float x1, float y1, color_t color) = 0;
	virtual void RenderTexture(TextureId texture, irect_t srcRect, rect_t dstRect, color_t color = 0xffffffff) = 0;
	virtual void RenderFilledRect(rect_t rect, color_t color) = 0;
};
