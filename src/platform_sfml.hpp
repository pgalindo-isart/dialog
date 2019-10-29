#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "platform.hpp"

#include "renderer_sfml.hpp"

class Platform
{
public:
    bool running = true;
    float deltaTime = 0.f;
    PlatformIO io = {};

public:
    Platform(const char* windowTitle, int width, int height);
    Renderer& getRenderer() { return renderer; }
    void startFrame();
    void display();

private:
    sf::RenderWindow window;
    sf::Clock deltaClock;

    RendererSFML renderer; // Order is important, RendererSFML needs sf::RenderWindow
};