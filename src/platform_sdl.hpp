
#pragma once

#include <SDL2/SDL.h>

#include "platform.hpp"

class Renderer;

class Platform
{
public:
    float deltaTime = 0.f;
    bool running = true;
    PlatformIO io = {};

public:
    Platform(const char* windowTitle, int width, int height);
    ~Platform();
    Renderer& getRenderer() { return *rendererWrapper; }
    void startFrame();
    void display();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Renderer* rendererWrapper;

    // Members for internal functions
    int previousTime = 0;
};
