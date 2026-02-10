
#pragma once

#include "Math/MathUtils.h"
#include "UI/HUD/HUDSkin.h"

/////////////////////
// HUDSkinTemplate //
/////////////////////
class HUDSkinTemplate : public IHUDSkin
{
private:
    float screen_w = 0.0f;
    float screen_h = 0.0f;
    float center_x = 0.0f;
    float center_y = 0.0f;
    float lane_depth_px = 200.0f;
    float metronome_pulse = 0.0f;

public:
    // - compute lane depth and reticle position
    // - update any animation phases using dt_sec
    void BeginFrame(const float screen_width, const float screen_height, const float current_beat, const float dt_sec) override
    {
        screen_w = screen_width;
        screen_h = screen_height;
        center_x = screen_w * 0.5f;
        center_y = screen_h * 0.5f;
        lane_depth_px = (screen_w < screen_h ? screen_w : screen_h) * 0.4f;

        static int last_beat = -1;
        int beat_now = static_cast<int>(current_beat);
        if (beat_now != last_beat)
        {
            last_beat = beat_now;
            metronome_pulse = 1.0f;
        }
        metronome_pulse = ClampFloat(metronome_pulse - dt_sec * 3.0f, 0.0f, 1.0f);
    }

    // draw background
    void DrawBackground() override
    {
    }

    // draw the lanes
    void DrawLanes() override
    {
    }

    // draw a single note entity
    // use entity.depth to place it between reticle (0) and lane end (1)
    // use entity.size to modify size
    void DrawEntity(const HUDSkinEntity&) override
    {
    }

    // draw the target
    // this is where hits are evaluated visually
    void DrawReticle() override
    {
        (void)metronome_pulse;
    }

    GameUI::Colour GetLaneColour(InputLane) const override
    {
        return GameUI::COLOUR_WHITE;
    }

    float GetLaneDepthPixels() const override
    {
        return lane_depth_px;
    }

    // override if HUD does not use all 4 lanes
    uint8_t GetActiveLanesMask() const override
    {
        return 0x0F;
    }

    // override if lane endpoints are not the default reticle + depth
    void SetLaneEnd(const InputLane lane, float& x, float& y) const override
    {
        IHUDSkin::SetLaneEnd(lane, x, y);
    }

    ////////////////
    // Ghost Pool //
    /////////////////////////
    // Disabled by default //
    /////////////////////////
    bool GhostsEnabled() const override
    {
        return false;
    }

    // origin of the ghost pool path
    void SetGhostPathOrigin(InputLane, float& x, float& y) const override
    {
        SetReticleCenter(x, y);
    }

    // how far away from the lane end ghosts should start
    float GetGhostTravelDistance() const override
    {
        return GetLaneDepthPixels() * 0.5f;
    }

    // draw a ghost note
    // depth is 0 - 1 along the ghost path
    void DrawGhost(const float x, const float y, const InputLane lane, float depth_normalized, const float alpha, const float radius) const override
    {
        GameUI::Colour base = GetLaneColour(lane);
        GameUI::Colour c = { base.red * alpha, base.green * alpha, base.blue * alpha };
        GameUI::DrawFilledCircle(x, y, radius, c, 12);
    }
};
