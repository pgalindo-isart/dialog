
#include <SDL2/SDL.h>

#include "renderer_sdl.hpp"

RendererSDL::RendererSDL(SDL_Renderer& renderer)
    : renderer(renderer)
{

}

TextureId RendererSDL::CreateTexture(const char* filename)
{
    fprintf(stderr, "Not implemented: Cannot create texture %s\n", filename);
    return -1;
}

TextureId RendererSDL::CreateTexture(int width, int height, unsigned char* pixels)
{
    SDL_Texture* texture = SDL_CreateTexture(&renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    SDL_UpdateTexture(texture, nullptr, pixels, width * sizeof(uint32_t));
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    printf("Texture created: %dx%d\n", width, height);
    this->textures.push_back(texture);

    return (int)(this->textures.size() - 1);
}

void RendererSDL::RenderLine(float x0, float y0, float x1, float y1, color_t color)
{
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(&renderer, x0, y0, x1, y1);
}

void RendererSDL::RenderTexture(TextureId textureId, irect_t srcRect, rect_t dstRect, color_t color)
{
    if (textureId == -1)
        return;

    // TODO: Remove hardcoded values
    if (!dstRect.intersects(rect_t{0, 0, 1280, 720}))
        return;

    SDL_Texture* texture = this->textures[textureId];

    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);

    SDL_Rect src = { srcRect.x, srcRect.y, srcRect.w, srcRect.h };
    SDL_Rect dst = { (int)(dstRect.x + 0.5f), (int)(dstRect.y + 0.5f), (int)(dstRect.w + 0.5f), (int)(dstRect.h + 0.5f) };
    SDL_RenderCopy(&renderer, texture, &src, &dst);
}

void RendererSDL::RenderFilledRect(rect_t rect, color_t color)
{
    SDL_SetRenderDrawColor(&renderer, color.r, color.g, color.b, color.a);
    SDL_Rect dst = { (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h };
    SDL_RenderFillRect(&renderer, &dst);
}