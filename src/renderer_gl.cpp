
#include "renderer_gl.hpp"

TextureId RendererGL::CreateTexture(int width, int height, unsigned char* pixels)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLTexture tex;
    tex.width = width;
    tex.height = height;
    tex.texture = texture;
    textures.push_back(tex);

    return textures.size() - 1;
}

void RendererGL::RenderLine(float x0, float y0, float x1, float y1, color_t color)
{
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_LINES);

    glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
    
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);

    glEnd();
}

void RendererGL::RenderTexture(TextureId texture, irect_t srcRect, rect_t dstRect, color_t color)
{
    glEnable(GL_TEXTURE_2D);
    const GLTexture& tex = textures[texture];

    glBindTexture(GL_TEXTURE_2D, tex.texture);
    glBegin(GL_QUADS);

    //glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
    
    glTexCoord2f(srcRect.x / (float)tex.width, srcRect.y / (float)tex.height);
    glVertex3f(dstRect.x, dstRect.y, 0.f);

    glTexCoord2f((srcRect.x + srcRect.w) / (float)tex.width, srcRect.y / (float)tex.height);
    glVertex3f(dstRect.x + dstRect.w, dstRect.y, 0.f);

    glTexCoord2f((srcRect.x + srcRect.w) / (float)tex.width, (srcRect.y + srcRect.h) / (float)tex.height);
    glVertex3f(dstRect.x + dstRect.w, dstRect.y + dstRect.h, 0.f);

    glTexCoord2f(srcRect.x / (float)tex.width, (srcRect.y + srcRect.h) / (float)tex.height);
    glVertex3f(dstRect.x, dstRect.y + dstRect.h, 0.f);

    glEnd();
}

void RendererGL::RenderFilledRect(rect_t rect, color_t color)
{
    return;
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
    
    glVertex3f(rect.x, rect.y, 0.f);
    glVertex3f(rect.x + rect.w, rect.y, 0.f);
    glVertex3f(rect.x + rect.w, rect.y + rect.h, 0.f);
    glVertex3f(rect.x, rect.y + rect.h, 0.f);

    glEnd();
}