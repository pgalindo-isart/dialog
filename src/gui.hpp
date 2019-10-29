#pragma once

#include "font.hpp"

class Renderer;

struct GuiIO
{
	bool mouseLeft;
	vec2_t mousePos;
};

class Gui
{
public:
	Gui(Renderer& renderer);
	~Gui();

	// Very basic immediate gui impl
	// Immediate gui functions starts with Im prefix
	void ImNewFrame(GuiIO io);
	void ImDrawBackground(int items);
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
	Renderer & renderer;
	GuiIO io;
	GuiIO prevIO;

	// Immediate mode state (for ImXXX functions)
	struct imgui_t
	{
		float penX = 0.f;
		float penY = 0.f;

		float beginY;

		float itemWidth = 200.f;
		float itemHeight = 22.f;
		float itemSpacing = 3.f;
		float textOffsetY = 15.f;
		float textPaddingX = 7.f;
	};

	imgui_t imState;
	TextureId whiteTexture; // Texture used to draw filled rects

	// Fonts loaded
	Font debugFont;
	Font uiFont;
};