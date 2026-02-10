#pragma once

#include "UI/HUD/HUDSkin.h"
#include "UI/Core/GameUI.h"
#include "Engine/Engine.h"

//////////////////
// Default Skin //
//////////////////
class SinglePlayerSkin : public IHUDSkin
{
private:
    float screen_width = 0.0f;
    float screen_height = 0.0f;
    float center_x = 0.0f;
    float center_y = 0.0f;
    float lane_depth_px = 0.0f;
    float metronome_pulse = 0.0f;
    float scanline_phase = 0.0f;

public:
    // initial calculations per frame
    void BeginFrame(const float screen_w, const float screen_h, const float current_beat, const float dt_sec) override
    {
        this->screen_width = screen_w;
        this->screen_height = screen_h;
        center_x = screen_width * 0.5f;
        center_y = screen_height * 0.5f;
        lane_depth_px = (screen_width < screen_height ? screen_width : screen_height) * 0.30f;

        static int last_beat = -1;
        const int beat_now = static_cast<int>(floorf(current_beat));
        if (beat_now != last_beat)
        {
            last_beat = beat_now;
            metronome_pulse = 1.0f;
        }
        metronome_pulse = ClampFloat(metronome_pulse - dt_sec * 3.0f, 0.0f, 1.0f);
        scanline_phase += dt_sec * 6.0f;
    }
    
    void DrawBackground() override
    {
        for (int i = 40; static_cast<float>(i) < screen_width - 40; i += 40)
        {
            for (int j = 40; static_cast<float>(j) < screen_height - 40; j += 40)
            {
                GameUI::DrawHexagon(static_cast<float>(i), static_cast<float>(j), 10, 0.1f, 0.1f, 0.1f);
            }
        }
        GameUI::DrawScanlines(scanline_phase, 1.0f);
    }

    void DrawLanes() override
    {
        InputLane lanes[4] = {InputLane::Up, InputLane::Right, InputLane::Down, InputLane::Left};

        for (const InputLane lane : lanes)
        {
            const Vector2 lane_direction = GetLaneDirection(lane);
            const GameUI::Colour lane_color = GetLaneColour(lane);

            Engine::DrawLine(center_x, center_y, center_x + lane_direction.x_position * lane_depth_px,
                          center_y + lane_direction.y_position * lane_depth_px, lane_color.red * 0.25f,
                          lane_color.green * 0.25f, lane_color.blue * 0.25f);
            
            for (int marker_index = 1; marker_index <= 5; ++marker_index)
            {
                const float marker = static_cast<float>(marker_index) / 5.0f;
                const float marker_x = center_x + lane_direction.x_position * lane_depth_px * marker;
                const float marker_y = center_y + lane_direction.y_position * lane_depth_px * marker;
                
                GameUI::DrawFilledCircle(marker_x, marker_y, 1.5f,{ lane_color.red * 0.2f,
                                   lane_color.green * 0.2f, lane_color.blue * 0.2f }, 8);
            }
        }
    }

    void DrawEntity(const HUDSkinEntity& entity) override
    {
        if (entity.consumed) return;

        const Vector2 lane_direction = GetLaneDirection(entity.lane);
        const float entity_x = center_x + lane_direction.x_position * lane_depth_px * entity.depth_normalized;
        const float entity_y = center_y + lane_direction.y_position * lane_depth_px * entity.depth_normalized;

        const float entity_size = 4.0f + entity.size * 4.0f;
        const GameUI::Colour lane_color = GetLaneColour(entity.lane);
        GameUI::DrawFilledCircle(entity_x, entity_y, entity_size, lane_color, 16);
    }

    void DrawReticle() override
    {
        const float pulse_scale = 1.0f + metronome_pulse * 0.25f;
        GameUI::DrawCircleLines(center_x, center_y, 14.0f * pulse_scale, 0.8f, 0.8f, 0.8f, 24);
        Engine::DrawLine(center_x - 6.0f, center_y, center_x + 6.0f, center_y, 0.6f, 0.6f, 0.6f);
        Engine::DrawLine(center_x, center_y - 6.0f, center_x, center_y + 6.0f, 0.6f, 0.6f, 0.6f);
    }

    bool GhostsEnabled() const override { return true; }

    void SetGhostPathOrigin(const InputLane lane, float& out_x, float& out_y) const override
    {
        SetLaneEnd(lane, out_x, out_y);
    }

    float GetGhostTravelDistance() const override { return lane_depth_px * 0.55f; }

    void DrawGhost(const float position_x, const float position_y, const InputLane lane, float /*depth_normalized*/, const float alpha, const float size) const override
    {
        const GameUI::Colour lane_color = GetLaneColour(lane);
        GameUI::DrawShapeOutline(position_x, position_y, size + 1.0f, { lane_color.red * alpha, lane_color.green * alpha, lane_color.blue * alpha }, 1.0f, 8);

    }

    float GetLaneDepthPixels() const override { return lane_depth_px; }

    uint8_t GetActiveLanesMask() const override { return 0x0F; }

    GameUI::Colour GetLaneColour(const InputLane lane) const override
    {
        switch (lane)
        {
            case InputLane::Up: return GameUI::COLOUR_Y;
            case InputLane::Right: return GameUI::COLOUR_B;
            case InputLane::Down: return GameUI::COLOUR_A;
            case InputLane::Left: return GameUI::COLOUR_X;
            default: return GameUI::COLOUR_WHITE;
        }
    }
};
