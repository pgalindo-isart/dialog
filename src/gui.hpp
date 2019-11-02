#pragma once

#include "font.hpp"

#include "im_api.h"

class Renderer;

class Gui
{
public:
	Gui(Renderer& renderer);
	~Gui();

	// Very basic immediate gui impl
	// Immediate gui functions starts with Im prefix
	void ImNewFrame(im_io_t io);
	void ImSetPalette(const unsigned int* palette);
	void ImSetItemWidth(float width);
	void ImText(const char* format, ...);
	void ImTextV(const char* format, va_list args);
	bool ImSliderFloat(const char* text, float* value, float min, float max);
	bool ImCheckBox(const char* text, bool* value);

	const Font& DebugFont() const { return debugFont; }
	const Font& UIFont() const { return uiFont; }

	// Draw functions
	void DrawText(const Font& font, const char* text, float x, float y);
	void DrawFilledRect(rect_t rect, color_t color);

private:
	Renderer& renderer;

	bool ImMouseJustPressed();
	bool ImMouseJustReleased();
	bool ImMouseInside(rect_t rect);

	// Immediate mode state (for ImXXX functions)
	struct imgui_t
	{
		float penX = 0.f;
		float penY = 0.f;

		float beginY;

        int itemCount = 0;
		float itemWidth = 200.f;
		float itemHeight = 22.f;
		float itemSpacing = 3.f;
		float textOffsetY = 15.f;
		float textPaddingX = 7.f;

		int itemId = 0;
		int selectedId = -1;
		
		unsigned int palette[IM_PAL_SIZE];

		im_io_t io;
	};

	imgui_t imState;
	imgui_t imPrevState;

	TextureId whiteTexture; // Texture used to draw filled rects

	// Fonts loaded
	Font debugFont;
	Font uiFont;
};
