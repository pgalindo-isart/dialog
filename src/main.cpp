
#include <cstring>

#if !defined(PLATFORM_SDL) && !defined(PLATFORM_SFML)
#error "No platform defined, use -DPLATFORM_SDL or -DPLATFORM_SFML"
#endif

#include "platform.hpp"
#include "enable_high_perf_gpu.hpp"

#include "gui.hpp"     // Gui functions
#include "maths.hpp"   // Math functions

// Reference function, to duplicate
void animatedDialog(Renderer& renderer, Gui& gui, const char* text, float advance);

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FUNCTION TO IMPLEMENT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// (at the end of the file)
//
// text:    Text to display
// advance: value between 0.0 and 1.0
void myAnimatedDialog(Renderer& renderer, Gui& gui, const char* text, float advance);

int main(int argc, char* argv[])
{
    Platform p("Animated dialog", 1280, 720);

    // Scope for GameRenderer and Gui
    {
        Gui gui(p.getRenderer());
        gui.ImSetItemWidth(1000);

        const char* text = 
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod\n"
            "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim\n"
            "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea\n"
            "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate\n"
            "velit esse cillum dolore eu fugiat nulla pariatur.";
        int textLen = strlen(text);
        float advance = 0.f;
        bool animate = true;
        float charsPerSeconds = 25.f;

        while (p.running)
        {
            p.startFrame();
            if (animate)
                advance += p.deltaTime * charsPerSeconds / textLen;

            im_io_t guiIO; // Gui need mouse pos and mouse left button to work
            guiIO.mouseLeft = p.io.mouseLeft;
            guiIO.mousePos[0] = p.io.mousePos.x;
            guiIO.mousePos[1] = p.io.mousePos.y;

            gui.ImNewFrame(guiIO);

            gui.ImText("FPS: %.2f", 1.f / p.deltaTime);
            gui.ImSliderFloat("charsPerSeconds", &charsPerSeconds, 10.f, 100.f);
            gui.ImCheckBox("animate", &animate);
            gui.ImSliderFloat("advance", &advance, 0.f, 1.f);
            if (gui.ImButton("reset"))
                advance = 0.f;
            
            gui.DrawText(gui.DebugFont(), "Make this work =>", 90, 365);
            gui.DrawText(gui.DebugFont(), "     Like this =>", 90, 622);
            
            // Implement this function
            myAnimatedDialog(p.getRenderer(), gui, text, advance);

            // To look like this
            animatedDialog(p.getRenderer(), gui, text, advance);

            p.display();
        }
    }

    return 0;
}

void myAnimatedDialog(Renderer& renderer, Gui& gui, const char* text, float advance)
{
    const Font& font = gui.UIFont();

    // Background coords
    float boxWidth  = 820.f;
    float boxHeight = 165.f;
    float x = (1280 - boxWidth) / 2.f;
    float y = (720 - boxHeight) / 2.f;

    // Render background
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

        // Start position
        float startX = x + 10.f;
        float startY = y + 28.f;

        // AdvanceY is the distance to advance for each new line
        float advanceY = (font.ascent - font.descent + font.lineGap) * font.scale * lineSpacingScale;

        // Position of the baseline
        // It is updated each time a glyph is drawn
        float penX = startX;
        float penY = startY;

        // Cursor that point to the character to display
        const char* cursor = text;
        while (*cursor != '\0')
        {
            int32_t charCode = (unsigned char)*cursor;
            switch (charCode)
            {
            case '\n':
                penX = startX;
                penY += advanceY;
                break;

            default:
                {
                    // Get glyph position and texture coords
                    irect_t src;
                    rect_t dst;
                    font.GetGlyphCoords(charCode, &penX, &penY, &src, &dst);
                    color_t color = 0xffffffff;

                    // =======================================================================
                    //                    ALL THE CHANGES CAN BE MADE HERE
                    //        Here you can modulate dst.x and dst.y to change position.
                    //                 And modulate color.a to change alpha.
                    // =======================================================================
                    // EXAMPLE: Modulate color.a with sin()
                    color.a = f32_linearRemap(-1.f, 1.f, f32_sin(advance * 140.f), 150.f, 255.f, false);
                    // sin() return a value between [-1,1] that we remap to [150,255] (color value)
                    // We multiply by 140.f to accelerate the movement

                    //
                    // Some useful math functions: (see src/maths.h)
                    //   float f32_lerp(float x, float y, float a);
                    //         f32_lerp(10, 20, 0.3);
                    //             Interpolate between 10 and 20 => return 13.33
                    //   
                    //   float f32_linearRemap(float inMin, float inMax, float v, float outMin, float outMax, bool clamp);
                    //         f32_linearRemap(10, 20, 14, 0, 200);
                    //             Remap 15 from [10,20] (50%), to [0,200] => return 100
                    //   
                    //   float f32_step(float edge, float x);
                    //             Return 0 if x < edge, else 1
                    //
                    //   float f32_smoothStep(float edge0, float edge1, float x)
                    //             Performs smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1.
                    // =======================================================================

                    // Draw glyph using atlas
                    renderer.RenderTexture(font.glyphAtlas, src, dst, color);

                    // Kerning (special space between two characters)
                    if (cursor[1] != '\0')
                        penX += font.scale * font.GetKerningAdvance(cursor[0], cursor[1]);
                }
            }

            cursor++;
        }
    }
}
