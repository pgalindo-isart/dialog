
#include <cstring>

#include "maths.hpp"
#include "gui.hpp"

void animatedDialog(Renderer& renderer, Gui& gui, const char* text, float advance)
{
    static float overshoot     = 3.f;
    static float valueStart    = -4.f;
    static float valueEnd      = 0.f;
    static float entryDelay    = 2.f;
    static float entryDuration = 8.f;
    static bool  debug         = false;
    gui.ImCheckBox("debug", &debug);
    gui.ImSliderFloat("valueStart",    &valueStart,  -50.f, 50.f);
    gui.ImSliderFloat("valueEnd",      &valueEnd,    -50.f, 50.f);
    gui.ImSliderFloat("overshoot",     &overshoot,   -50.f, 50.f);
    gui.ImSliderFloat("entryDelay",    &entryDelay,  -10.f, 10.f);
    gui.ImSliderFloat("entryDuration", &entryDuration, 0.f, 100.f);

    // TODO: Handle UTF-8
    const Font& font = gui.UIFont();

    float boxWidth  = 820.f;
    float boxHeight = 165.f;
    float x = (1280 - boxWidth) / 2.f;
    float y = 720 - boxHeight - 20.f;

    // Render bg
    {
        float bgAlpha = 0.8f;
        rect_t bg = { x, y, boxWidth, boxHeight };
        color_t bgColor = 0x333333FF;
        bgColor.a = (u8)(bgAlpha * 0xff);
        gui.DrawFilledRect(bg, bgColor);
    }

    // Render text
    {
        float lineSpacingScale = 1.25f;
        int len = (int)strlen(text);

        float minEntryDelay = 0.f;    // Using this value to garantee that the animation start when advance value is 0.0
        float maxEntryDuration = 8.f; // Using this value to garantee that the animation start when advance value is 1.0
        float nbCharsDisplayed = f32_lerp(minEntryDelay, (float)len + maxEntryDuration, advance);

        float startX = x + 10.f;
        float startY = y + 28.f;
        int advanceY = (int)((font.ascent - font.descent + font.lineGap) * font.scale * lineSpacingScale);

        float penX = startX;
        float penY = startY;

        const char* cursor = text;
        while (*cursor != '\0')
        {
            int charIndex = (int)(cursor - text);

            int32_t charCode = (unsigned char)*cursor;
            switch (charCode)
            {
            case '\n':
                penX = startX;
                penY += advanceY;
                break;

            default:
            {
                irect_t src;
                rect_t dst;
                font.GetGlyphCoords(charCode, &penX, &penY, &src, &dst);
                color_t color = 0xffffffff;

                // Color animation
                {
                    const float start    = 0.f;
                    const float end      = 1.f;
                    const float delay    = 0.f;
                    const float duration = 6.f;
                    float amount = f32_smoothStep(charIndex + delay, charIndex + delay + duration, nbCharsDisplayed);
                    color.a = (u8)(f32_lerp(start, end, amount * 255.f));
                }
                // Y-Offset animation
                {
                    float amount = f32_smoothStep(charIndex + entryDelay, charIndex + entryDelay + entryDuration, nbCharsDisplayed);
                    dst.y += f32_lerp(valueStart, valueEnd, tween_overshoot(amount, overshoot));
                }
                
                if (debug)
                {
                    // Glyph go from black to red when charIndex reach nbCharsDisplayed
                    color.a = 0xff;
                    color.g = color.b = 0x00;
                    float red = f32_lerp(valueStart, valueEnd, f32_step(charIndex, nbCharsDisplayed));
                    red = f32_linearRemap(valueStart, valueEnd, red, 0.f, 255.f, true);
                    color.r = (u8)red;

                    // Same logic, but smoothly increase green value with parameters
                    float animAmount = f32_smoothStep(charIndex + entryDelay, charIndex + entryDelay + entryDuration, nbCharsDisplayed);
                    float green = f32_lerp(valueStart, valueEnd, animAmount);
                    green = f32_linearRemap(valueStart, valueEnd, green, 0.f, 255.f, true);
                    color.g = (u8)green;
                }

                // Draw!
                renderer.RenderTexture(font.glyphAtlas, src, dst, color);

                // Kerning
                if (cursor[1] != '\0')
                    penX += font.scale * font.GetKerningAdvance(cursor[0], cursor[1]);
            }
            }

            cursor++;
        }
    }
}
