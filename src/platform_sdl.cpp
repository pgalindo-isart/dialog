
#include "renderer_sdl.hpp"

#include "platform_sdl.hpp"

Platform::Platform(const char* windowTitle, int width, int height)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    rendererWrapper = new RendererSDL(*renderer);
}

Platform::~Platform()
{
    delete rendererWrapper;
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}

void Platform::startFrame()
{
    unsigned int currentTime = SDL_GetTicks();
    deltaTime = (previousTime == 0) ? 1.f / 60.f : (currentTime - previousTime) / 1000.f;
    previousTime = currentTime;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            running = false;
        break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
                io.mouseLeft = true;
        break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                io.mouseLeft = false;
        break;
        case SDL_MOUSEMOTION:
            io.mousePos.x = event.motion.x;
            io.mousePos.y = event.motion.y;
        break;

        default:;
        }
    }

    SDL_SetRenderDrawColor(renderer, 0x1e, 0x1e, 0x1e, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void Platform::display()
{
    SDL_RenderPresent(renderer);
}