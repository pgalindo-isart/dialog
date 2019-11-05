
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
    
    unsigned int pal[IM_PAL_SIZE] = {
        0x252526FF, // IM_PAL_BG
        0x383838FF, // IM_PAL_ITEM_BG
        0x007ACCFF, // IM_PAL_ITEM_CURSOR
        0x1F8BD3FF, // IM_PAL_ITEM_CURSOR_HOVER
        0x72B7E5FF, // IM_PAL_ITEM_CURSOR_HIGHLIGHT
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

bool Gui::ImMouseJustPressed()
{
    return (imState.io.mouseLeft && !imPrevState.io.mouseLeft);
}

bool Gui::ImMouseJustReleased()
{
    return (!imState.io.mouseLeft && imPrevState.io.mouseLeft);
}

bool Gui::ImMouseInside(rect_t rect)
{
    return rect.contains({ imState.io.mousePos[0], imState.io.mousePos[1] });
}

void Gui::ImAdvancePen()
{
    imState.maxPenX = f32_max(imState.maxPenX, imState.penX);
    
    if (imState.itemId == 0)
    {
        // First line
        imState.penY = 10.f;
        imState.penX = 10.f;
    }
    else if (imState.skipNextAdvancePen)
    {
        imState.skipNextAdvancePen = false;
    }
    else
    {
        // Go to new line
        imState.penX = 10.f;
        imState.penY += imState.itemHeight + imState.itemSpacing;
    }
}

void Gui::ImNewFrame(im_io_t io)
{
    imPrevState = imState;
    imState.itemId = 0;
    ImAdvancePen();

    imPrevState.penY += imState.itemHeight + imState.itemSpacing;
    
    // Draw background based on previous frame item count
    rect_t bg;
    bg.x = imState.penX - imState.itemSpacing;
    bg.y = imState.penY - imState.itemSpacing;
    bg.w = (imPrevState.maxPenX - imState.penX) + 2 * imState.itemSpacing;
    bg.h = (imPrevState.penY - imState.penY) + imState.itemSpacing;

    color_t bgColor = imState.palette[IM_PAL_BG];
    this->DrawFilledRect(bg, bgColor);

    imState.io = io;
    imState.maxPenX = 0.f;
    imState.itemCount = 0;
}

void Gui::ImSetPalette(const unsigned int* pal)
{
    memcpy(imState.palette, pal, IM_PAL_SIZE * sizeof(unsigned int));
}

void Gui::ImSetItemWidth(float itemWidth)
{
    imState.itemWidth = itemWidth;
}

void Gui::ImSameLine(float spacing)
{
    imState.skipNextAdvancePen = true;

    if (spacing == -1.f)
        spacing = imState.itemSpacing;

    imState.penX += spacing;
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
    ImAdvancePen();

    // TODO: Make it safe
    char buf[255];
    vsprintf(buf, format, args);
    rect_t rect = this->DrawText(debugFont, buf, imState.penX, imState.penY + imState.textOffsetY);

    imState.penX += rect.w;
    imState.itemCount++;
    imState.itemId++;
}

bool Gui::ImSliderFloat(const char* text, float* value, float min, float max)
{
    ImAdvancePen();

    bool changed = false;

    // Background dimensions
    rect_t bg;
    bg.x = imState.penX;
    bg.y = imState.penY;
    bg.w = imState.itemWidth;
    bg.h = imState.itemHeight;

    bool hovered = ImMouseInside(bg);

    // Cursor dimensions
    float cursor_margin = 3.f;
    float cursor_width = 10.f;

    // If mouse is pressed inside, value will change
    float ratio;
    if (imState.focusedId == -1 && ImMouseJustPressed() && hovered)
    {
        imState.focusedId = imState.itemId;
    }
    else if (imState.focusedId == imState.itemId && ImMouseJustReleased())
    {
        imState.focusedId = -1;
    }

    bool focused = imState.focusedId == imState.itemId;
    if (focused)
    {
        float cursor_center_x_left = bg.x + cursor_margin + cursor_width / 2.f;
        float cursor_center_x_right = bg.x + bg.w - cursor_margin - cursor_width / 2.f;

        //ratio = f32_clamp(f32_invLerp(cursor_center_x_left, cursor_center_x_right, imState.io.mousePos.x), 0.f, 1.f);
        ratio = f32_linearRemap(cursor_center_x_left, cursor_center_x_right, imState.io.mousePos[0], 0.f, 1.f, true);
        *value = f32_lerp(min, max, ratio);
        changed = true;
    }
    else
    {
        ratio = f32_clamp((*value - min) / (max - min), 0.f, 1.f);
    }

    // Draw bg
    this->DrawFilledRect(bg, imState.palette[IM_PAL_ITEM_BG]);

    // Draw cursor
    rect_t cursor;
    cursor.x = f32_lerp(bg.x + cursor_margin, bg.x + bg.w - cursor_margin - cursor_width, ratio);
    cursor.y = bg.y + 1 * cursor_margin;
    cursor.h = bg.h - 2 * cursor_margin;
    cursor.w = cursor_width;
    this->DrawFilledRect(cursor, imState.palette[focused ? IM_PAL_ITEM_CURSOR_HIGHLIGHT : (hovered ? IM_PAL_ITEM_CURSOR_HOVER : IM_PAL_ITEM_CURSOR)]);

    // Draw number text
    char valueStr[255];
    sprintf(valueStr, "%s: %.2f", text, *value);
    this->DrawText(debugFont, valueStr, imState.penX + 6.f, imState.penY + imState.textOffsetY);

    imState.penX += bg.w;
    imState.itemCount++;
    imState.itemId++;

    return changed;
}

bool Gui::ImCheckBox(const char* text, bool* value)
{
    ImAdvancePen();

    bool changed = false;

    // Background dimensions
    rect_t bg;
    bg.x = imState.penX;
    bg.y = imState.penY;
    bg.w = imState.itemHeight;
    bg.h = imState.itemHeight;

    bool hovered = ImMouseInside(bg);
    
    // Cursor dimensions
    float cursor_margin = 6.f;

    // Draw bg
    this->DrawFilledRect(bg, imState.palette[IM_PAL_ITEM_BG]);

    if (imState.focusedId == -1 && ImMouseJustPressed() && hovered)
    {
        imState.focusedId = imState.itemId;
    }
    else if (imPrevState.focusedId == imState.itemId && ImMouseJustReleased())
    {
        if (hovered)
        {
            *value = !*value;
            changed = true;
        }
        imState.focusedId = -1;
    }

    // Draw cursor
    rect_t cursor;
    cursor.x = bg.x + cursor_margin;
    cursor.y = bg.y + cursor_margin;
    cursor.h = bg.h - 2 * cursor_margin;
    cursor.w = bg.w - 2 * cursor_margin;

    // Change color alpha depending on state (selected, true, false)
    color_t color = imState.palette[hovered ? IM_PAL_ITEM_CURSOR_HOVER : IM_PAL_ITEM_CURSOR];
    u8 alpha;
    if (imState.focusedId == imState.itemId)
        alpha = color.a / 2;
    else
        alpha = *value ? color.a : 0;
    color.a = alpha;
    
    this->DrawFilledRect(cursor, color);

    // Draw text
    rect_t textRect = this->DrawText(debugFont, text, imState.penX + bg.w + imState.textPaddingX, imState.penY + imState.textOffsetY);

    imState.penX = imState.penX + bg.w + imState.textPaddingX + textRect.w;
    imState.itemCount++;
    imState.itemId++;

    return changed;
}

bool Gui::ImButton(const char* text)
{
    ImAdvancePen();

    bool buttonPressed = false;

    rect_t textSize = GetTextRect(debugFont, text);

    // Background dimensions
    rect_t bg;
    bg.x = imState.penX;
    bg.y = imState.penY;
    bg.w = textSize.w + 2 * imState.textPaddingX;
    bg.h = imState.itemHeight;

    bool hovered = ImMouseInside(bg);
    
    if (imState.focusedId == -1 && ImMouseJustPressed() && hovered)
    {
        imState.focusedId = imState.itemId;
    }
    else if (imState.focusedId == imState.itemId && ImMouseJustReleased())
    {
        imState.focusedId = -1;
        if (ImMouseInside(bg))
            buttonPressed = true;
    }

    bool focused = (imState.focusedId == imState.itemId);

    // Draw bg
    this->DrawFilledRect(bg, imState.palette[focused ? IM_PAL_ITEM_CURSOR_HIGHLIGHT : (hovered ? IM_PAL_ITEM_CURSOR_HOVER : IM_PAL_ITEM_CURSOR)]);
    
    // Draw text
    this->DrawText(debugFont, text, imState.penX + imState.textPaddingX, imState.penY + imState.textOffsetY);

    imState.penX += bg.w;
    imState.itemCount++;
    imState.itemId++;

    return buttonPressed;
}

rect_t Gui::DrawText(const Font& font, const char* text, float x, float y)
{
    float penX = x;
    float penY = y;
    
    rect_t rect = { x, y, 0, 0 };

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
                rect = rect.grow(dst);

                // Draw!
                renderer.RenderTexture(font.glyphAtlas, src, dst);

                // Kerning
                if (cursor[1] != '\0')
                    penX += font.scale * font.GetKerningAdvance(cursor[0], cursor[1]);
            }
        }

        cursor++;
    }

    return rect;
}

rect_t Gui::GetTextRect(const Font& font, const char* text) const
{
    rect_t rect = {};
    float penX = 0.f;
    float penY = 0.f;

    int advanceY = (int)((font.ascent - font.descent + font.lineGap) * font.scale);

    // TODO: Handle UTF-8
    const char* cursor = text;
    while (*cursor != '\0')
    {
        int32_t charCode = (unsigned char)*cursor;
        switch (charCode)
        {
        case '\n':
            penX = 0.f;
            penY += advanceY;
            break;

        default:
            {
                irect_t src;
                rect_t dst;
                font.GetGlyphCoords(charCode, &penX, &penY, &src, &dst);
                rect = rect.grow(dst);

                // Kerning
                if (cursor[1] != '\0')
                    penX += font.scale * font.GetKerningAdvance(cursor[0], cursor[1]);
            }
        }

        cursor++;
    }

    return rect;
}