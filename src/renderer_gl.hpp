#pragma once

#ifdef __EMSCRIPTEN__
#include <GL/gl.h>
#else
#include <glad/glad.h>
#endif

#include "renderer.hpp"

struct GLTexture
{
    int width;
    int height;
    GLuint texture;
};

class RendererGL : public Renderer
{
public:
	virtual TextureId CreateTexture(int width, int height, unsigned char* pixels) override;

	virtual void RenderLine(float x0, float y0, float x1, float y1, color_t color) override;
	virtual void RenderTexture(TextureId texture, irect_t srcRect, rect_t dstRect, color_t color = 0xffffffff) override;
	virtual void RenderFilledRect(rect_t rect, color_t color) override;

private:
	std::vector<GLTexture> textures;
};
