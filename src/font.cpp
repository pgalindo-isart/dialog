
#include <fstream>
#include <cassert>

#define STB_TRUETYPE_IMPLEMENTATION
#include "font.hpp"

Font::Font(Renderer& renderer, const char* filename, float fontSize)
{
    // 128x128 is a good size of 13px height fontSize
    this->atlasWidth = (int)((128.f / 13.f) * fontSize);
    this->atlasHeight = this->atlasWidth;

    // Load ttfBuffer
    {
        std::ifstream fin(filename, std::ios::out | std::ios::binary);
        fin.seekg(0, std::ios_base::end);
        size_t size = fin.tellg();
        fin.seekg(0, std::ios_base::beg);

        this->ttfBuffer.resize(size);
        fin.read(&this->ttfBuffer[0], size);
        fin.close();
    }

    // Load font info
    {
        unsigned char* fontData = (uint8_t*)this->ttfBuffer.c_str();
        stbtt_InitFont(&this->info, fontData, stbtt_GetFontOffsetForIndex(fontData, 0));
        // Load metrics
        this->scale = stbtt_ScaleForPixelHeight(&this->info, fontSize);
        stbtt_GetFontVMetrics(&this->info, &this->ascent, &this->descent, &this->lineGap);
    }

    // Pack font into a luminance texture (only one channel)
    std::vector<uint8_t> luminance(this->atlasWidth * this->atlasHeight);
    {
        stbtt_pack_context spc;
        stbtt_PackBegin(&spc, &luminance[0], this->atlasWidth, this->atlasHeight, 0, 1, nullptr);

        struct { int start; int end; } ranges[] =
        {
            { 0xFFFD, 0xFFFD }, // Specials: Replacement character
            { 0x0020, 0x007F }, // Basic Latin
            { 0x00A0, 0x00FF }, // Latin-1 Supplement
        };

        // Calculate nbChars in all ranges
        int nbChars = 0;
        for (int i = 0; i < (int)ARRAY_SIZE(ranges); ++i)
            nbChars += ranges[i].end - ranges[i].start + 1;
        this->packedChars.resize(nbChars);

        // Create all stbtt_pack_range and store it in font.ranges
        int charIndex = 0;
        for (int i = 0; i < (int)ARRAY_SIZE(ranges); ++i)
        {
            int charsInRange = ranges[i].end - ranges[i].start + 1;

            stbtt_pack_range range = {};
            range.first_unicode_codepoint_in_range = ranges[i].start;
            range.array_of_unicode_codepoints = nullptr;
            range.num_chars = charsInRange;
            range.font_size = fontSize;
            range.chardata_for_range = &this->packedChars[charIndex];

            charIndex += charsInRange;
            this->ranges.push_back(range);
        }

        // Pack and render ranges
        std::vector<stbrp_rect> rects(nbChars);
        stbtt_PackFontRangesGatherRects(&spc, &this->info, &this->ranges[0], (int)this->ranges.size(), &rects[0]);
        stbtt_PackFontRangesPackRects(&spc, &rects[0], nbChars);
        stbtt_PackFontRangesRenderIntoRects(&spc, &this->info, &this->ranges[0], (int)this->ranges.size(), &rects[0]);

        stbtt_PackEnd(&spc);
    }

    // Convert luminance texture to 32bits texture
    {
        std::vector<uint32_t> rgbaBitmap(this->atlasWidth * this->atlasHeight);
        for (int y = 0; y < this->atlasHeight; ++y)
        {
            for (int x = 0; x < this->atlasHeight; ++x)
            {
                uint8_t alpha = luminance[x + y * this->atlasWidth];

                // Cast uint32_t to uint8_t to access to the different channels
                uint8_t* color = (uint8_t*)&rgbaBitmap[x + y * this->atlasWidth];
                color[0] = 0xff;
                color[1] = 0xff;
                color[2] = 0xff;
                color[3] = alpha;
            }
        }

        this->glyphAtlas = renderer.CreateTexture(this->atlasWidth, this->atlasHeight, (unsigned char*)&rgbaBitmap[0]);
    }
}

void Font::GetGlyphCoords(int32_t charCode, float* penX, float* penY, irect_t* src, rect_t* dst) const
{
    assert(!this->ranges.empty());

    // Search in which range this char belongs
    const stbtt_pack_range* curRange = nullptr;
    for (const stbtt_pack_range& range : this->ranges)
    {
        if (charCode >= range.first_unicode_codepoint_in_range
            && charCode <  range.first_unicode_codepoint_in_range + range.num_chars)
        {
            curRange = &range;
            break;
        }
    }

    // If no range found, use a special char (replaceable char)
    if (curRange == nullptr)
    {
        curRange = &this->ranges[0];
        charCode = curRange->first_unicode_codepoint_in_range;
    }

    // Get packed quad coordinates
    stbtt_aligned_quad q;
    stbtt_GetPackedQuad(curRange->chardata_for_range, this->atlasWidth, this->atlasHeight,
        charCode - curRange->first_unicode_codepoint_in_range,
        penX, penY, &q, 0);

    // Use quad coords to generate texture rect
    *src = {
        (int)((q.s0          * this->atlasWidth) + 0.5f), // +0.5 to round to next integer
        (int)((q.t0          * this->atlasHeight) + 0.5f),
        (int)(((q.s1 - q.s0) * this->atlasWidth) + 0.5f),
        (int)(((q.t1 - q.t0) * this->atlasHeight) + 0.5f)
    };
    *dst = {
        q.x0,
        q.y0,
        q.x1 - q.x0,
        q.y1 - q.y0
    };
}

int Font::GetKerningAdvance(int32_t char0, int32_t char1) const
{
    return stbtt_GetCodepointKernAdvance(&info, char0, char1);
}
