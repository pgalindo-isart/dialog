
#include <iostream>
#include <cassert>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "renderer_sfml.hpp"

RendererSFML::RendererSFML(sf::RenderWindow& window)
    : window(window)
{
}

TextureId RendererSFML::CreateTexture(int width, int height, unsigned char* pixels)
{
    sf::Texture texture;
    if (!texture.create(width, height))
    {
        fprintf(stderr, "Error: Cannot create texture of size %dx%d'\n", width, height);
        return -1;
    }

    texture.update(pixels);
    texture.setSmooth(true);
    textures.push_back(texture);

    return (int)textures.size()-1;
}

void RendererSFML::RenderLine(float x0, float y0, float x1, float y1, color_t color)
{
    sf::Color vcolor(color.value);
    sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(x0, y0), vcolor),
        sf::Vertex(sf::Vector2f(x1, y1), vcolor)
    };

    window.draw(line, 2, sf::Lines);
}

void RendererSFML::RenderTexture(TextureId textureId, irect_t srcRect, rect_t dstRect, color_t color)
{
    if (textureId == -1)
        return;

    // TODO: Remove hardcoded values
    if (!dstRect.intersects(rect_t{0, 0, 1280, 720}))
        return;

    assert(textureId < (int)textures.size());

    sf::Texture& texture = textures[textureId];

    // Use vertices instead of sprite
    sf::RectangleShape shape;
    shape.setTexture(&texture);
    shape.setFillColor(sf::Color(color.value));
    shape.setTextureRect(sf::IntRect(srcRect.x, srcRect.y, srcRect.w, srcRect.h));
    shape.setPosition(sf::Vector2f(dstRect.x, dstRect.y));
    shape.setSize(sf::Vector2f(dstRect.w, dstRect.h));

    window.draw(shape);
}

void RendererSFML::RenderFilledRect(rect_t rect, color_t color)
{
    sf::RectangleShape shape;
    shape.setPosition(sf::Vector2f(rect.x, rect.y));
    shape.setSize(sf::Vector2f(rect.w, rect.h));
    shape.setFillColor(sf::Color(color.value));

    window.draw(shape);
}
