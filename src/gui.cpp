
#include <cstdarg>
#include <cstring>

#include "maths.hpp"

#include "gui.hpp"

Gui::Gui(Renderer& renderer)
    : renderer(renderer)
    , debugFont(renderer, "media/ProggyClean.ttf", 13.f)
    , uiFont(renderer, "media/Renogare-Regular.otf", 24.f)
{
    uint32_t whitePixel = 0xffffffff;
    whiteTexture = renderer.CreateTexture(1, 1, (unsigned char*)&whitePixel);
    
    const unsigned int pal[IM_PAL_SIZE] = {
        0x252526FF, // IM_PAL_BG
        0x383838FF, // IM_PAL_ITEM_BG
        0x007accFF, // IM_PAL_ITEM_CURSOR
        0xCCCCCCFF, // IM_PAL_ITEM_TEXT
    };
    ImSetPalette(pal);
}

Gui::~Gui()
{
}

void Gui::DrawFilledRect(rect_t rect, color_t color)
{
    irect_t src = { 0, 0, 1, 1 };
    renderer.RenderTexture(whiteTexture, src, rect, color);
}

void Gui::ImNewFrame(im_io_t io)
{
    prevIO = this->io;
    this->io = io;
    imState.penX = 10.f;
    imState.penY = 10.f;
}

void Gui::ImSetPalette(const unsigned int* pal)
{
    memcpy(imState.palette, pal, IM_PAL_SIZE * sizeof(unsigned int));
}

void Gui::ImSetItemWidth(float itemWidth)
{
    imState.itemWidth = itemWidth;
}

void Gui::ImBackground(int items)
{
    rect_t bg;
    bg.x = imState.penX - imState.itemSpacing;
    bg.y = imState.penY - imState.itemSpacing;
    bg.w = imState.itemWidth + 2 * imState.itemSpacing;
    bg.h = (imState.itemHeight * items + imState.itemSpacing * (items - 1)) + 2 * imState.itemSpacing;

    color_t bgColor = imState.palette[IM_PAL_BG];
    this->DrawFilledRect(bg, bgColor);
}

void Gui::ImText(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    this->ImTextV(format, args);
    va_end(args);
}

void Gui::ImTextV(const char* format, va_list args)
{
    // TODO: Make it safe
    char buf[255];
    vsprintf(buf, format, args);
    this->DrawText(debugFont, buf, imState.penX, imState.penY + imState.textOffsetY);
    imState.penY += imState.itemHeight + imState.itemSpacing;
}

bool Gui::ImSliderFloat(const char* text, float* value, float min, float max)
{
    bool changed = false;

    // Background dimensions
    rect_t bg;
    bg.x = imState.penX;
    bg.y = imState.penY;
    bg.w = imState.itemWidth;
    bg.h = imState.itemHeight;

    // Cursor dimensions
    float cursor_margin = 3.f;
    float cursor_width = 10.f;

    // If mouse is pressed inside, value will change
    float ratio;
    if (io.mouseLeft && bg.contains({ io.mousePos[0], io.mousePos[1] }))
    {
        float cursor_center_x_left = bg.x + cursor_margin + cursor_width / 2.f;
        float cursor_center_x_right = bg.x + bg.w - cursor_margin - cursor_width / 2.f;

        //ratio = f32_clamp(f32_invLerp(cursor_center_x_left, cursor_center_x_right, io.mousePos.x), 0.f, 1.f);
        ratio = f32_linearRemap(cursor_center_x_left, cursor_center_x_right, io.mousePos[0], 0.f, 1.f, true);
        *value = f32_lerp(min, max, ratio);
        changed = true;
    }
    else
    {
        ratio = f32_clamp((*value - min) / (max - min), 0.f, 1.f);
    }

    // Render bg
    this->DrawFilledRect(bg, imState.palette[IM_PAL_ITEM_BG]);

    // Render cursor
    rect_t cursor;
    cursor.x = f32_lerp(bg.x + cursor_margin, bg.x + bg.w - cursor_margin - cursor_width, ratio);
    cursor.y = bg.y + 1 * cursor_margin;
    cursor.h = bg.h - 2 * cursor_margin;
    cursor.w = cursor_width;
    this->DrawFilledRect(cursor, imState.palette[IM_PAL_ITEM_CURSOR]);

    // Render number text
    char valueStr[255];
    sprintf(valueStr, "%s: %.2f", text, *value);
    this->DrawText(debugFont, valueStr, imState.penX + 6.f, imState.penY + imState.textOffsetY);

    imState.penY += imState.itemHeight + imState.itemSpacing;

    return changed;
}

bool Gui::ImCheckBox(const char* text, bool* value)
{
    bool changed = false;

    // Background dimensions
    rect_t bg;
    bg.x = imState.penX;
    bg.y = imState.penY;
    bg.w = imState.itemHeight;
    bg.h = imState.itemHeight;

    // Render bg
    this->DrawFilledRect(bg, imState.palette[IM_PAL_ITEM_BG]);

    if ((io.mouseLeft && !prevIO.mouseLeft) && bg.contains({ io.mousePos[0], io.mousePos[1] }))
    {
        *value = !*value;
        changed = true;
    }

    if (*value)
    {
        // Cursor dimensions
        float cursor_margin = 6.f;

        rect_t cursor;
        cursor.x = bg.x + cursor_margin;
        cursor.y = bg.y + cursor_margin;
        cursor.h = bg.h - 2 * cursor_margin;
        cursor.w = bg.w - 2 * cursor_margin;
        this->DrawFilledRect(cursor, imState.palette[IM_PAL_ITEM_CURSOR]);
    }

    // Render number text
    this->DrawText(debugFont, text, imState.penX + bg.w + imState.textPaddingX, imState.penY + imState.textOffsetY);

    imState.penY += imState.itemHeight + imState.itemSpacing;

    return changed;
}

void Gui::DrawText(const Font& font, const char* text, float x, float y)
{
    float penX = x;
    float penY = y;

    int advanceY = (int)((font.ascent - font.descent + font.lineGap) * font.scale);

    // TODO: Handle UTF-8
    const char* cursor = text;
    while (*cursor != '\0')
    {
        int32_t charCode = (unsigned char)*cursor;
        switch (charCode)
        {
        case '\n':
            penX = x;
            penY += advanceY;
            break;

        default:
            {
                irect_t src;
                rect_t dst;
                font.GetGlyphCoords(charCode, &penX, &penY, &src, &dst);

                // Draw!
                renderer.RenderTexture(font.glyphAtlas, src, dst);

                // Kerning
                if (cursor[1] != '\0')
                    penX += font.scale * font.GetKerningAdvance(cursor[0], cursor[1]);
            }
        }

        cursor++;
    }

}