#pragma once
#include "Engine/Engine.h"
#include <string>
#include <cmath>
//////////////////////////////////////////////////////
// VARIOUS UI TOOLS USED DURING EARLY PROTOTYPING //
//////////////////////////////////////////////////////
class GameUI
{
public:
    struct Colour { float red, green, blue; };
    static constexpr Colour COLOUR_A = { 0.0f, 1.0f, 0.0f };    
    static constexpr Colour COLOUR_B = { 1.0f, 0.0f, 0.0f };    
    static constexpr Colour COLOUR_X = { 0.0f, 0.5f, 1.0f };    
    static constexpr Colour COLOUR_Y = { 1.0f, 0.8f, 0.0f };    
    static constexpr Colour COLOUR_WHITE = { 1.0f, 1.0f, 1.0f };
    static constexpr Colour COLOUR_BLACK = { 0.0f, 0.0f, 0.0f };
    static constexpr Colour COLOUR_GRAY  = { 0.5f, 0.5f, 0.5f };

    // drawing primitives
    static void DrawBox(float origin_x, float origin_y, float width_px, float height_px, float red, float green, float blue);
    static void DrawCircleLines(float center_x, float center_y, float radius, float red, float green, float blue, int segments = 64);


    // draw a circular button with text inside
    static void DrawButton(float center_x, float center_y, float radius_px, const char* label, const Colour& colour);

    // draw the controller button layout
    static void DrawControllerLayout(float center_x, float center_y, float scale = 1.0f);

    // draw text with an outline for better visibility
    static void PrintTextWithOutline(float text_x, float text_y, const std::string& text, const Colour& colour, const Colour& outline_colour = COLOUR_BLACK);

    // draw a filled circle (using polygon approximation)
    static void DrawFilledCircle(float center_x, float center_y, float radius, const Colour& colour, int segments = 24);

    // draw a circle outline
    static void DrawShapeOutline(float center_x, float center_y, float radius, const Colour& colour, float thickness = 1.0f, int segments = 24);

    // draw a hexagon
    static void DrawHexagon(float center_x, float center_y, float radius, float red, float green, float blue);


    // draw a filled rectangle
    static void DrawRectangle(float origin_x, float origin_y, float width, float height, const Colour& colour, bool wireframe = false);


    // draw CRT scanlines effect
    static void DrawScanlines(float scanline_phase, float alpha = 0.05f, float start_y = 0.0f, float end_y = -1.0f);

    // draw a healthbar
    static void DrawHealthBar(float origin_x, float origin_y, float width, float height, float current_value, float max_value,
                              float fill_red = 0.0f, float fill_green = 1.0f, float fill_blue = 0.0f,
                              float border_red = 1.0f, float border_green = 1.0f, float border_blue = 1.0f);


    static void DrawCornerBrackets(float origin_x, float origin_y, float width_px, float height_px, float length_px, float red, float green, float blue);
    static void DrawArc(float center_x, float center_y, float start, float end, float radius, float red, float green, float blue, int segments = 24);
    static void DrawArcWrapped(float center_x, float center_y, float start, float end, float radius, float red, float green, float blue, int segments = 24);
    static void DrawMeterWindow(float origin_x, float origin_y, float width_px, float height_px, float start, float end, float red, float green, float blue);
    static void DrawStar(float center_x, float center_y, float size, float red, float green, float blue, float phase = 0.0f);
    static void DrawPixelBox(float origin_x, float origin_y, float width_px, float height_px, float red, float green, float blue);

    
    static void PrintCenteredText(const char* text, const bool colour_cycle = false, float red = 1.0f, float green = 1.0f, float blue = 1.0f)
    {
        constexpr float char_width = 12.0f;
        constexpr float line_height = 18.0f;

        // measure text length
        int text_length = 0;
        while (text[text_length] != '\0')
        {
            ++text_length;
        }

        const float text_width = static_cast<float>(text_length) * char_width;

        constexpr float center_x = APP_VIRTUAL_WIDTH * 0.5f;
        constexpr float center_y = APP_VIRTUAL_HEIGHT * 0.5f;

        const float text_x = center_x - text_width * 0.5f;
        const float text_y = center_y - line_height * 0.5f;

        if (colour_cycle)
        {
            static int frame_count = 0;
            ++frame_count;

            const float time = static_cast<float>(frame_count) * 0.05f;
            red = 0.5f + 0.5f * sinf(time);
            green = 0.5f + 0.5f * sinf(time + 2.0f);
            blue = 0.5f + 0.5f * sinf(time + 4.0f);
        }
        Engine::Print(text_x, text_y, text, red, green, blue);
    }
};
