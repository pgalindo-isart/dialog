
#include <cassert>

#include "renderer_gl.hpp"
#include "gui.hpp"

#include "im_api.h"

static Renderer* g_renderer = nullptr;
static Gui* g_gui = nullptr;

static bool im_valid()
{
    return g_renderer != nullptr && g_gui != nullptr;
}

void im_init()
{
    assert(!im_valid());
    g_renderer = new RendererGL();
    g_gui = new Gui(*g_renderer);
}

void im_shutdown()
{
    assert(im_valid());
    delete g_gui;
    delete g_renderer;
}

void im_newFrame(im_io_t io)
{
    assert(im_valid());
    g_gui->ImNewFrame(io);
}

void im_setPalette(const unsigned int* palette)
{
    assert(im_valid());
    g_gui->ImSetPalette(palette);
}

void im_setItemWidth(float itemWidth)
{
    assert(im_valid());
    g_gui->ImSetItemWidth(itemWidth);
}

void im_sameLine(float spacing)
{
    assert(im_valid());
    g_gui->ImSameLine(spacing);
}

void im_text(const char* format, ...)
{
    assert(im_valid());
    va_list args;
    va_start(args, format);
    im_text_v(format, args);
    va_end(args);
}

void im_text_v(const char* format, va_list args)
{
    assert(im_valid());
    g_gui->ImTextV(format, args);
}

void im_sliderFloat(const char* text, float* value, float min, float max)
{
    assert(im_valid());
    g_gui->ImSliderFloat(text, value, min, max);
}

bool im_checkbox(const char* text, bool* value)
{
    assert(im_valid());
    return g_gui->ImCheckBox(text, value);
}

bool im_button(const char* text)
{
    assert(im_valid());
    return g_gui->ImButton(text);
}

void im_get_text_rect(const char* text, float* x, float* y, float* w, float* h)
{
    assert(im_valid());
    rect_t rect = g_gui->GetTextRect(g_gui->DebugFont(), text);
    *x = rect.x;
    *y = rect.y;
    *w = rect.w;
    *h = rect.h;
}

void im_draw_text(float x, float y, int r, int g, int b, const char* text)
{
    assert(im_valid());
    color_t color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = 0xff;
    g_gui->DrawText(g_gui->DebugFont(), text, x, y, color);
}