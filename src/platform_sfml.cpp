
#include "platform_sfml.hpp"

Platform::Platform(const char* windowTitle, int width, int height)
    : window(sf::VideoMode(width, height), windowTitle, sf::Style::Titlebar | sf::Style::Close)
    , renderer(window)
{
	window.setVerticalSyncEnabled(true);
}

void Platform::startFrame()
{
    deltaTime = deltaClock.restart().asSeconds();
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
        case sf::Event::Closed:
            running = false;
            window.close();
            break;
        case sf::Event::MouseMoved:
            io.mousePos.x = (float)event.mouseMove.x;
            io.mousePos.y = (float)event.mouseMove.y;
            break;
        case sf::Event::MouseButtonPressed:
            if (event.mouseButton.button == sf::Mouse::Button::Left)
                io.mouseLeft = true;
            break;
        case sf::Event::MouseButtonReleased:
            if (event.mouseButton.button == sf::Mouse::Button::Left)
                io.mouseLeft = false;
            break;
        default:;
        }
    }

    window.clear(sf::Color(0x444444ff));
}

void Platform::display()
{
    window.display();
}
