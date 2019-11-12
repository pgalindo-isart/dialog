
#pragma once

#include <cstdarg>

#ifdef __cplusplus
extern "C" {
#endif

struct im_io_t
{
	bool mouseLeft;
	float mousePos[2];
};

enum im_palette_t
{
    IM_PAL_BG,
    IM_PAL_ITEM_BG,
    IM_PAL_ITEM_CURSOR,
    IM_PAL_ITEM_CURSOR_HOVER,
    IM_PAL_ITEM_CURSOR_HIGHLIGHT,
    IM_PAL_ITEM_TEXT,

    IM_PAL_SIZE
};

void im_init();
void im_shutdown();

void im_newFrame(im_io_t io);

void im_setPalette(const unsigned int* palette);
float im_setItemWidth(float itemWidth);

void im_sameLine(float spacing = -1.f);
void im_text(const char* format, ...);
void im_text_v(const char* format, va_list args);
bool im_sliderFloat(const char* text, float* value, float min, float max);
bool im_sliderFloat3(const char* text, float* values, float min, float max);
bool im_sliderFloat4(const char* text, float* values, float min, float max);
bool im_checkbox(const char* text, bool* value);
bool im_button(const char* text);
void im_get_text_rect(const char* text, float* x, float* y, float* w, float* h);
void im_draw_text(float x, float y, int r, int g, int b, const char* text);

#ifdef __cplusplus
}
#endif