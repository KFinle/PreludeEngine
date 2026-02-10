#pragma once

#include "UI/HUD/HUDSkin.h"
#include "UI/Core/GameUI.h"
#include "Engine/Engine.h"

/////////////////////
// Two Player Skin //
////////////////////////////////////////
// Uses only the left and right lanes //
////////////////////////////////////////
class TwoPlayerSkin : public IHUDSkin
{
private:
    static constexpr float TargetOffsetRatio = 0.36f;
    static constexpr float LaneMarkerRadiusRatio = 0.010f;
    static constexpr int LaneMarkerCount = 9;
    static constexpr float EmitterRadiusRatio = 0.022f;
    static constexpr float EmitterGlowRadiusRatio = 0.06f;
    static constexpr float TargetRingRadiusRatio = 0.036f;
    static constexpr float BeatPulseDecayPerSec = 3.4f;
    static constexpr float FlareSpeed = 1.15f;
    static constexpr float NoteBaseRadiusRatio = 0.012f;
    static constexpr float CenterCrossHalfRatio = 0.018f;
    static constexpr int NoteSegments = 8;
    static constexpr int ReticleSegments = 8;

    float screen_width = 0.0f;
    float screen_height = 0.0f;
    float min_dimension = 0.0f;
    float center_x = 0.0f;
    float center_y = 0.0f;
    float lane_depth_px = 0.0f;
    float player_offset_x = 0.0f;
    float left_target_x = 0.0f;
    float right_target_x = 0.0f;
    float beat_pulse = 0.0f;
    float flare_phase = 0.0f;

    void GetPlayerTarget(const InputLane lane, float& target_x, float& target_y) const
    {
        target_x = (lane == InputLane::Left) ? left_target_x : right_target_x;
        target_y = center_y;
    }

public:
    void BeginFrame(const float width, const float height, const float current_beat, const float dt_sec) override
    {
        this->screen_width = width;
        this->screen_height = height;
        min_dimension = (width < height) ? width : height;
        center_x = width * 0.5f;
        center_y = height * 0.5f;
        player_offset_x = width * TargetOffsetRatio;
        lane_depth_px = player_offset_x;
        left_target_x = center_x - player_offset_x;
        right_target_x = center_x + player_offset_x;

        static int last_beat = -1;
        const int beat_now = static_cast<int>(floorf(current_beat));
        if (beat_now != last_beat)
        {
            last_beat = beat_now;
            beat_pulse = 1.0f;
        }
        beat_pulse = ClampFloat(beat_pulse - dt_sec * BeatPulseDecayPerSec, 0.0f, 1.0f);
        flare_phase += dt_sec * FlareSpeed;
    }

    void DrawBackground() override
    {
        const float emitter_radius = min_dimension * EmitterRadiusRatio;
        const float emitter_glow_radius = min_dimension * EmitterGlowRadiusRatio * (0.7f + 0.3f * beat_pulse);

        constexpr GameUI::Colour emitter_glow = { 0.2f, 0.2f, 0.25f };
        GameUI::DrawCircleLines(center_x, center_y, emitter_glow_radius, emitter_glow.red, emitter_glow.green, emitter_glow.blue, ReticleSegments);
        
        GameUI::DrawFilledCircle(center_x, center_y, emitter_radius, { 0.9f, 0.9f, 0.9f }, NoteSegments);

        const float cross_half = min_dimension * CenterCrossHalfRatio;
        const float cross_alpha = 0.6f * (0.7f + 0.3f * sinf(flare_phase));
        Engine::DrawLine(center_x - cross_half, center_y, center_x + cross_half, center_y, cross_alpha, cross_alpha, cross_alpha);
        Engine::DrawLine(center_x, center_y - cross_half, center_x, center_y + cross_half, cross_alpha, cross_alpha, cross_alpha);
    }

    void DrawLanes() override
    {
        InputLane lanes[2] = { InputLane::Left, InputLane::Right };
        for (const InputLane lane : lanes)
        {
            float target_x = 0.0f;
            float target_y = 0.0f;
            GetPlayerTarget(lane, target_x, target_y);

            const GameUI::Colour lane_colour = GetLaneColour(lane);
            const float line_alpha = 0.18f + 0.38f * beat_pulse;
            Engine::DrawLine(center_x, center_y, target_x, target_y, lane_colour.red * line_alpha, lane_colour.green * line_alpha, lane_colour.blue * line_alpha);

            for (int marker_index = 1; marker_index <= LaneMarkerCount; ++marker_index)
            {
                const float marker_ratio = static_cast<float>(marker_index) / static_cast<float>(LaneMarkerCount);
                const float marker_x = LerpFloat(center_x, target_x, marker_ratio);
                const float marker_y = LerpFloat(center_y, target_y, marker_ratio);
                const float marker_radius = min_dimension * LaneMarkerRadiusRatio * (1.0f - marker_ratio * 0.55f);
                const float marker_alpha = 0.25f + 0.35f * (1.0f - marker_ratio);

                GameUI::DrawFilledCircle(marker_x, marker_y, marker_radius,
                                {
                                             lane_colour.red * marker_alpha, lane_colour.green * marker_alpha,
                                             lane_colour.blue * marker_alpha
                                         }, NoteSegments);
            }
        }
    }

    void DrawEntity(const HUDSkinEntity& entity) override
    {
        if (entity.consumed) return;

        float target_x = 0.0f;
        float target_y = 0.0f;
        GetPlayerTarget(entity.lane, target_x, target_y);

        const float position_x = LerpFloat(target_x, center_x, entity.depth_normalized);
        const float position_y = LerpFloat(target_y, center_y, entity.depth_normalized);

        const float base_radius = min_dimension * NoteBaseRadiusRatio;
        const float magnitude_scale = 1.0f + entity.size * 0.75f;
        const float depth_scale = 1.0f - entity.depth_normalized * 0.45f;
        const float entity_radius = base_radius * magnitude_scale * depth_scale;

        const GameUI::Colour lane_color = GetLaneColour(entity.lane);
        const float glow_alpha = 0.4f + 0.6f * (1.0f - entity.depth_normalized);
        GameUI::DrawFilledCircle(position_x, position_y, entity_radius,
                                 { lane_color.red * glow_alpha, lane_color.green * glow_alpha, lane_color.blue * glow_alpha },
                                 NoteSegments);
        // GameUI::DrawCircleLines(position_x, position_y, entity_radius * 1.4f,
        //                         lane_color.red * 0.25f,
        //                         lane_color.green * 0.25f,
        //                         lane_color.blue * 0.25f,
        //                         ReticleSegments);
    }

    void DrawReticle() override
    {
        InputLane lanes[2] = { InputLane::Left, InputLane::Right };
        for (const InputLane lane : lanes)
        {
            float target_x = 0.0f;
            float target_y = 0.0f;
            GetPlayerTarget(lane, target_x, target_y);

            const GameUI::Colour lane_color = GetLaneColour(lane);
            const float ring_radius = min_dimension * TargetRingRadiusRatio * (1.0f + beat_pulse * 0.45f);
            const float ring_alpha = 0.35f + 0.45f * beat_pulse;
            GameUI::DrawCircleLines(target_x, target_y, ring_radius,
                                    lane_color.red * ring_alpha, lane_color.green * ring_alpha, lane_color.blue * ring_alpha,
                                    ReticleSegments);
        }
    }

    GameUI::Colour GetLaneColour(const InputLane lane) const override
    {
        if (lane == InputLane::Left) return GameUI::COLOUR_X;
        if (lane == InputLane::Right) return GameUI::COLOUR_B;
        return GameUI::COLOUR_WHITE;
    }

    float GetLaneDepthPixels() const override { return lane_depth_px; }

    uint8_t GetActiveLanesMask() const override
    {
        return GetLaneBit(InputLane::Left) | GetLaneBit(InputLane::Right);
    }

    void SetLaneTarget(const InputLane lane, float& target_x, float& target_y) const override
    {
        GetPlayerTarget(lane, target_x, target_y);
    }

    void SetLaneEnd(const InputLane lane, float& end_x, float& end_y) const override
    {
        (void)lane;
        end_x = center_x;
        end_y = center_y;
    }

    
    bool GhostsEnabled() const override { return true; }

void SetGhostPathOrigin(const InputLane lane, float& origin_x, float& origin_y) const override
{
    origin_x = center_x;
    if (lane == InputLane::Left) origin_y = center_y + (screen_height * 0.18f); 
    else origin_y = center_y - (screen_height * 0.18f);
}


    float GetGhostTravelDistance() const override { return lane_depth_px * 0.4f; }

    void DrawGhost(const float pos_x, const float pos_y, const InputLane lane, float,
                   const float alpha, const float size) const override
    {
        const GameUI::Colour color = GetLaneColour(lane);
        GameUI::DrawFilledCircle(pos_x, pos_y, size * 0.9f,
            { color.red * alpha, color.green * alpha, color.blue * alpha }, 10);
    }

















};
