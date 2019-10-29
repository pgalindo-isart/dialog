#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "stb_truetype.h"

#include "renderer.hpp"

class Font
{
public:
	// Need renderer to create texture atlas
	Font(Renderer& renderer, const char* filename, float fontSize);

	// Get texture rect (src) and glyph quad to display (dst) for a char (charCode)
	// PenX and PenY are also advanced to the next position
	void GetGlyphCoords(int32_t charCode, float* penX, float* penY, irect_t* src, rect_t* dst) const;

	// Get kerning between 2 chars
	int GetKerningAdvance(int32_t char0, int32_t char1) const;

	// Font metrics
	float scale; // All metrics values (integers) have to be multiplied by this scale value
	int ascent;
	int descent;
	int lineGap;

	// Texture
	TextureId glyphAtlas;
private:
	std::string ttfBuffer; // ttf file in memory
	stbtt_fontinfo info;   // STB font info, (mapped on internal ttfBuffer)

	// Atlas and packing
	std::vector<stbtt_packedchar> packedChars;
	std::vector<stbtt_pack_range> ranges;

	int atlasWidth;
	int atlasHeight;
};
