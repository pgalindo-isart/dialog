#pragma once

#include "types.h"

struct PlatformIO
{
    bool mouseLeft;
    vec2_t mousePos;
};

#ifdef PLATFORM_SDL
#include "platform_sdl.hpp"
#endif

#ifdef PLATFORM_SFML
#include "platform_sfml.hpp"
#endif