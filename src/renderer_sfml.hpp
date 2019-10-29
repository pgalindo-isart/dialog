#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "renderer.hpp"

// Forward declaration
namespace sf
{
	class RenderWindow;
}

class RendererSFML : public Renderer
{
public:
	RendererSFML(sf::RenderWindow& window);

	virtual TextureId CreateTexture(const char* filename) override;
	virtual TextureId CreateTexture(int width, int height, unsigned char* pixels) override;

	virtual void RenderLine(float x0, float y0, float x1, float y1, color_t color) override;
	virtual void RenderTexture(TextureId texture, irect_t srcRect, rect_t dstRect, color_t color = 0xffffffff) override;
	virtual void RenderFilledRect(rect_t rect, color_t color) override;

private:
	sf::RenderWindow& window;
	std::vector<sf::Texture> textures;
};
