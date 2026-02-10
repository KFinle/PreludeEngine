#include "GameUI.h"
#include <vector>
#include <cmath>

#include "Math/MathUtils.h"

///////////////////////////////////////////////
// VARIOUS UI TOOLS USED DURING API PRACTICE //
///////////////////////////////////////////////

///////////////////
// Local helpers //
///////////////////
namespace
{

   const std::vector<float>& GetUnitCircle(int segments)
    {
        segments = IntMax(8, segments);

        static int cached_segments = 0;
        static std::vector<float> unit_points;

        if (cached_segments != segments)
        {
            cached_segments = segments;
            unit_points.clear();
            unit_points.reserve(static_cast<size_t>(segments + 1) * 2);

            for (int segment_index = 0; segment_index <= segments; ++segment_index)
            {
                const float angle = (static_cast<float>(segment_index) / static_cast<float>(segments)) * two_pi;
                unit_points.push_back(cosf(angle));
                unit_points.push_back(sinf(angle));
            }
        }
        return unit_points;
    }

    void DrawFilledPolygon(const std::vector<float>& vertices, const GameUI::Colour& color)
    {
        if (vertices.size() < 6) return;

        constexpr float depth_z = 0.0f;
        constexpr float w = 1.0f;
        constexpr bool wireframe = false;
        const float origin_x = vertices[0];
        const float origin_y = vertices[1];

        // triangle fan
        for (size_t vertex_index = 4; vertex_index + 1 < vertices.size(); vertex_index += 2)
        {
            const float prev_x = vertices[vertex_index - 2];
            const float prev_y = vertices[vertex_index - 1];
            const float next_x = vertices[vertex_index];
            const float next_y = vertices[vertex_index + 1];

            Engine::DrawTriangle(
                origin_x, origin_y, depth_z, w,
                prev_x, prev_y, depth_z, w,
                next_x, next_y, depth_z, w,
                color.red, color.green, color.blue,
                color.red, color.green, color.blue,
                color.red, color.green, color.blue,
                wireframe
            );
        }
    }
}

////////////////
// Primitives //
////////////////
void GameUI::DrawBox(const float x_px, const float y_px, const float width_px, const float height_px, const float red, const float green, const float blue)
{
    Engine::DrawLine(x_px, y_px, x_px + width_px, y_px, red, green, blue);
    Engine::DrawLine(x_px + width_px, y_px, x_px + width_px, y_px + height_px, red, green, blue);
    Engine::DrawLine(x_px + width_px, y_px + height_px, x_px, y_px + height_px, red, green, blue);
    Engine::DrawLine(x_px, y_px + height_px, x_px, y_px, red, green, blue);
}


void GameUI::DrawRectangle(const float x_px, const float y_px, const float width, const float height, const Colour& colour, const bool wireframe)
{
    const float depth_z = 0.0f;
    const float w = 1.0f;
    
    Engine::DrawTriangle(x_px, y_px, depth_z, w,
                      x_px + width, y_px, depth_z, w,
                      x_px + width, y_px + height, depth_z, w,
                      colour.red, colour.green, colour.blue,
                      colour.red, colour.green, colour.blue,
                      colour.red, colour.green, colour.blue,
                      wireframe);
    
    Engine::DrawTriangle(x_px, y_px, depth_z, w,
                      x_px + width, y_px + height, depth_z, w,
                      x_px, y_px + height, depth_z, w,
                      colour.red, colour.green, colour.blue,
                      colour.red, colour.green, colour.blue,
                      colour.red, colour.green, colour.blue,
                      wireframe);
}

void GameUI::DrawCircleLines(const float center_x, const float center_y, const float radius, const float red, const float green, const float blue, int segments)
{
    segments = IntMax(8, segments);

    const std::vector<float>& unit_points = GetUnitCircle(segments);
    float prev_x = center_x + unit_points[0] * radius;
    float prev_y = center_y + unit_points[1] * radius;

    for (int segment_index = 1; segment_index <= segments; ++segment_index)
    {
        const int point_index = segment_index * 2;
        const float point_x = center_x + unit_points[point_index] * radius;
        const float point_y = center_y + unit_points[point_index + 1] * radius;

        Engine::DrawLine(prev_x, prev_y, point_x, point_y, red, green, blue);
        prev_x = point_x;
        prev_y = point_y;
    }
}

////////////////////
// Buttons & Text //
////////////////////
void GameUI::DrawFilledCircle(const float center_x, const float center_y, const float radius, const Colour& colour, int segments)
{
    segments = IntMax(8, segments);

    static std::vector<float> vertices;
    vertices.clear();
    vertices.reserve(static_cast<size_t>(segments + 2) * 2);

    // center
    vertices.push_back(center_x);
    vertices.push_back(center_y);

    // ring
    const std::vector<float>& unit_points = GetUnitCircle(segments);
    for (int segment_index = 0; segment_index <= segments; ++segment_index)
    {
        const int point_index = segment_index * 2;
        vertices.push_back(center_x + unit_points[point_index] * radius);
        vertices.push_back(center_y + unit_points[point_index + 1] * radius);
    }

    DrawFilledPolygon(vertices, colour);
}

void GameUI::DrawShapeOutline(const float center_x, const float center_y, const float radius, const Colour& colour, const float thickness, int segments)
{
    segments = IntMax(8, segments);

    const std::vector<float>& unit_points = GetUnitCircle(segments);
    float prev_x = center_x + unit_points[0] * radius;
    float prev_y = center_y + unit_points[1] * radius;

    for (int segment_index = 1; segment_index <= segments; ++segment_index)
    {
        const int point_index = segment_index * 2;
        const float point_x = center_x + unit_points[point_index] * radius;
        const float point_y = center_y + unit_points[point_index + 1] * radius;

        // Cheap thickness: draw a few offset lines along the normal
        for (float offset = -thickness * 0.5f; offset <= thickness * 0.5f; offset += 0.5f)
        {
            const float normal_x = (point_y - center_y) / (radius == 0.0f ? 1.0f : radius);
            const float normal_y = (center_x - point_x) / (radius == 0.0f ? 1.0f : radius);

            Engine::DrawLine(
                prev_x + normal_x * offset, prev_y + normal_y * offset,
                point_x + normal_x * offset, point_y + normal_y * offset,
                colour.red, colour.green, colour.blue);
        }

        prev_x = point_x;
        prev_y = point_y;
    }
}

void GameUI::DrawButton(const float center_x, const float center_y, const float radius_px, const char* label, const Colour& colour)
{
    DrawFilledCircle(center_x, center_y, radius_px, colour);

    std::string label_str(label);
    PrintTextWithOutline(center_x - static_cast<float>(label_str.length()) * 6, center_y - 4, label_str, COLOUR_WHITE);
}

void GameUI::DrawControllerLayout(const float center_x, const float center_y, const float scale)
{
    const float button_radius  = 20 * scale;
    const float button_spacing = 60 * scale;

    const std::string mode_label = "SELECT DIFFICULTY";
    const float label_width = static_cast<float>(mode_label.length()) * 8;
    PrintTextWithOutline(center_x - label_width / 2, center_y - 200 * scale, mode_label, COLOUR_WHITE);

    // XBOX layout
    DrawButton(center_x, center_y - button_spacing, button_radius, "A", COLOUR_A);
    DrawButton(center_x - button_spacing, center_y, button_radius, "X", COLOUR_X);
    DrawButton(center_x + button_spacing, center_y, button_radius, "B", COLOUR_B);
    DrawButton(center_x, center_y + button_spacing, button_radius, "Y", COLOUR_Y);

    PrintTextWithOutline(center_x - 20 * scale, center_y - button_spacing - 40 * scale, "BRUTAL", COLOUR_WHITE);
    PrintTextWithOutline(center_x - button_spacing - 25 * scale, center_y + 5 * scale, "EASY", COLOUR_WHITE);
    PrintTextWithOutline(center_x + button_spacing - 25 * scale, center_y + 5 * scale, "HARD", COLOUR_WHITE);
    PrintTextWithOutline(center_x - 30 * scale, center_y + button_spacing + 40 * scale, "MEDIUM", COLOUR_WHITE);
}

void GameUI::PrintTextWithOutline(const float text_x, const float text_y, const std::string& text, const Colour& colour, const Colour& outline_colour)
{
    // outline
    Engine::Print(text_x - 1, text_y - 1, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x, text_y - 1, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x + 1, text_y - 1, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x - 1, text_y, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x + 1, text_y, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x - 1, text_y + 1, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x, text_y + 1, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);
    Engine::Print(text_x + 1, text_y + 1, text.c_str(), outline_colour.red, outline_colour.green, outline_colour.blue);

    Engine::Print(text_x, text_y, text.c_str(), colour.red, colour.green, colour.blue);
}

void GameUI::DrawHexagon(const float center_x, const float center_y, const float radius, const float red, const float green, const float blue)
{
    const int sides = 6;
    const float step = two_pi / static_cast<float>(sides);

    float prev_x = center_x + radius;
    float prev_y = center_y;

    for (int side_index = 1; side_index <= sides; ++side_index)
    {
        const float angle = step * static_cast<float>(side_index);
        const float point_x = center_x + cosf(angle) * radius;
        const float point_y = center_y + sinf(angle) * radius;

        Engine::DrawLine(prev_x, prev_y, point_x, point_y, red, green, blue);
        prev_x = point_x;
        prev_y = point_y;
    }
}


void GameUI::DrawScanlines(const float scanline_phase, const float alpha, const float start_y, float end_y)
{
    if (end_y < 0.0f) end_y = static_cast<float>(APP_VIRTUAL_HEIGHT);
    if (end_y <= start_y) return;

    for (int scanline_y = static_cast<int>(start_y); scanline_y < static_cast<int>(end_y); scanline_y += 4)
    {
        float line_alpha = alpha * (0.03f + 0.02f * sinf(scanline_phase + static_cast<float>(scanline_y) * 0.1f));
        Engine::DrawLine(0.0f, static_cast<float>(scanline_y), static_cast<float>(APP_VIRTUAL_WIDTH), static_cast<float>(scanline_y), line_alpha, line_alpha, line_alpha);
    }
}

void GameUI::DrawHealthBar(const float origin_x, const float origin_y, const float width, const float height, const float current_value, const float max_value,
                           const float fill_red, const float fill_green, const float fill_blue,
                           const float border_red, const float border_green, const float border_blue)
{
    const float ratio = (max_value > 0.0f) ? ClampFloat(current_value / max_value, 0.0f, 1.0f) : 0.0f;
    const float fill_width = width * ratio;

    const float depth_z = 0.0f;
    const float w = 1.0f;

    if (ratio > 0.0f)
    {
        Engine::DrawTriangle(
            origin_x, origin_y, depth_z, w,
            origin_x + fill_width, origin_y, depth_z, w,
            origin_x, origin_y + height, depth_z, w,
            fill_red, fill_green, fill_blue,
            fill_red, fill_green, fill_blue,
            fill_red, fill_green, fill_blue,
            false
        );
        
        Engine::DrawTriangle(
            origin_x + fill_width, origin_y, depth_z, w,
            origin_x + fill_width, origin_y + height, depth_z, w,
            origin_x, origin_y + height, depth_z, w,
            fill_red, fill_green, fill_blue,
            fill_red, fill_green, fill_blue,
            fill_red, fill_green, fill_blue,
            false
        );
    }

    Engine::DrawLine(origin_x, origin_y, origin_x + width, origin_y, border_red, border_green, border_blue);
    Engine::DrawLine(origin_x + width, origin_y, origin_x + width, origin_y + height, border_red, border_green, border_blue);
    Engine::DrawLine(origin_x + width, origin_y + height, origin_x, origin_y + height, border_red, border_green, border_blue);
    Engine::DrawLine(origin_x, origin_y + height, origin_x, origin_y, border_red, border_green, border_blue);
}


void GameUI::DrawArc(float center_x, float center_y, float start, float end, float radius, float red, float green, float blue, int segments)
{
    segments = IntMax(8, segments);

    const float two_pi = 2 * PI;
    const float start_angle = start * two_pi;
    const float end_angle = end * two_pi;

    for (int segment_index = 0; segment_index < segments; ++segment_index)
    {
        const float start_t = static_cast<float>(segment_index) / static_cast<float>(segments);
        const float end_t = static_cast<float>(segment_index + 1) / static_cast<float>(segments);

        const float angle_start = LerpFloat(start_angle, end_angle, start_t);
        const float angle_end = LerpFloat(start_angle, end_angle, end_t);

        const float start_x = center_x + cosf(angle_start) * radius;
        const float start_y = center_y + sinf(angle_start) * radius;
        const float end_x = center_x + cosf(angle_end) * radius;
        const float end_y = center_y + sinf(angle_end) * radius;

        Engine::DrawLine(start_x, start_y, end_x, end_y, red, green, blue);
    }
}

void GameUI::DrawArcWrapped(float center_x, float center_y, float start, float end, float radius, float red, float green, float blue, int segments)
{
    start = WrapPhase(start);
    end   = WrapPhase(end);

    if (start <= end)
    {
        DrawArc(center_x, center_y, start, end, radius, red, green, blue, segments);
    }
    else
    {
        const int half = IntMax(8, segments / 2);
        DrawArc(center_x, center_y, start, 1.0f, radius, red, green, blue, half);
        DrawArc(center_x, center_y, 0.0f,  end, radius, red, green, blue, half);
    }
}

void GameUI::DrawMeterWindow(float x_px, float y_px, float width_px, float height_px, float start, float end, float red, float green, float blue)
{
    start = WrapPhase(start);
    end   = WrapPhase(end);

    auto span = [&](float start_norm, float end_norm)
    {
        const float start_x = x_px + ClampFloat(start_norm, 0.0f, 1.0f) * width_px;
        const float end_x = x_px + ClampFloat(end_norm, 0.0f, 1.0f) * width_px;

        Engine::DrawLine(start_x, y_px,            end_x, y_px,            red, green, blue);
        Engine::DrawLine(start_x, y_px + height_px, end_x, y_px + height_px, red, green, blue);
    };

    if (start <= end)
    {
        span(start, end);
    }
    else
    {
        span(start, 1.0f);
        span(0.0f, end);
    }
}

void GameUI::DrawCornerBrackets(float x_px, float y_px, float width_px, float height_px, float length_px, float red, float green, float blue)
{
    // top-left
    Engine::DrawLine(x_px, y_px, x_px + length_px, y_px, red, green, blue);
    Engine::DrawLine(x_px, y_px, x_px, y_px + length_px, red, green, blue);

    // top-right
    Engine::DrawLine(x_px + width_px - length_px, y_px, x_px + width_px, y_px, red, green, blue);
    Engine::DrawLine(x_px + width_px, y_px, x_px + width_px, y_px + length_px, red, green, blue);

    // bottom-left
    Engine::DrawLine(x_px, y_px + height_px - length_px, x_px, y_px + height_px, red, green, blue);
    Engine::DrawLine(x_px, y_px + height_px, x_px + length_px, y_px + height_px, red, green, blue);

    // bottom-right
    Engine::DrawLine(x_px + width_px, y_px + height_px - length_px, x_px + width_px, y_px + height_px, red, green, blue);
    Engine::DrawLine(x_px + width_px - length_px, y_px + height_px, x_px + width_px, y_px + height_px, red, green, blue);
}
void GameUI::DrawPixelBox(float origin_x, float origin_y, float width_px, float height_px, float red, float green, float blue)
{
    const int px_size = 4;
    for (float pixel_y = origin_y; pixel_y < origin_y + height_px; pixel_y += static_cast<float>(px_size))
    {
        for (float pixel_x = origin_x; pixel_x < origin_x + width_px; pixel_x += static_cast<float>(px_size))
        {
            DrawBox(pixel_x, pixel_y, static_cast<float>(px_size), static_cast<float>(px_size), red, green, blue);
        }
    }
}

void GameUI::DrawStar(float center_x, float center_y, float size, float red, float green, float blue, float phase)
{
    const int rays = 4;

    for (int ray = 0; ray < rays; ++ray)
    {
        const float angle = (ray / static_cast<float>(rays)) * two_pi + phase;
        const float end_x = center_x + cosf(angle) * size;
        const float end_y = center_y + sinf(angle) * size;

        const float alpha = 0.6f + 0.4f * sinf(phase * 2.0f);
        Engine::DrawLine(center_x, center_y, end_x, end_y, red * alpha, green * alpha, blue * alpha);
    }
}
