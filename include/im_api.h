
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
void im_setItemWidth(float itemWidth);

void im_text(const char* format, ...);
void im_text_v(const char* format, va_list args);
void im_sliderFloat(const char* text, float* value, float min, float max);
bool im_checkbox(const char* text, bool* value);
bool im_button(const char* text);

#ifdef __cplusplus
}
#endif