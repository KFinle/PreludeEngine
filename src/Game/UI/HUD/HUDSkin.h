#pragma once

#include "Gameplay/Lanes.h"
#include "UI/Core/GameUI.h"

////////////////////////
// HUD Skin Interface //
/////////////////////////////////////////////////////////
// Abstract interface for different HUD visual styles. //
/////////////////////////////////////////////////////////

struct HUDSkinEntity
{
    uint32_t id = 0;
    InputLane lane;
    float depth_normalized;
    float size;
    bool consumed;
};

class IHUDSkin
{
public:
    virtual ~IHUDSkin() = default;

    // called once per frame before drawing entities
    virtual void BeginFrame(float screen_w, float screen_h, float current_beat, float dt_sec) = 0;

    // draw a single entity approaching along a lane
    virtual void DrawEntity(const HUDSkinEntity& entity) = 0;

    // draw all lane rails and centerlines
    virtual void DrawLanes() = 0;

    // draw the reticle/target
    virtual void DrawReticle() = 0;

    // draw any background or decorative elements
    virtual void DrawBackground() = 0;

    // get the lane direction
    virtual GameUI::Colour GetLaneColour(InputLane lane) const = 0;

    // get base lane depth for scaling
    virtual float GetLaneDepthPixels() const { return 200.0f; }

    // get active lanes mask (Up|Right|Down|Left)
    virtual uint8_t GetActiveLanesMask() const { return 0x0F; }

    // get per-lane target position
    virtual void SetLaneTarget(InputLane, float& target_x, float& target_y) const
    {
        SetReticleCenter(target_x, target_y);
    }

    // set reticle position
    virtual void SetReticleCenter(float& center_x, float& center_y) const
    {
        center_x = static_cast<float>(APP_VIRTUAL_WIDTH) * 0.5f;
        center_y = static_cast<float>(APP_VIRTUAL_HEIGHT) * 0.5f;
    }

    // set the outer lane end position
    virtual void SetLaneEnd(const InputLane lane, float& end_x, float& end_y) const
    {
        float center_x = 0.0f;
        float center_y = 0.0f;
        SetReticleCenter(center_x, center_y);
        const float angle = GetLaneAngle(lane);
        const float radius = GetLaneDepthPixels();
        end_x = center_x + cosf(angle) * radius;
        end_y = center_y + sinf(angle) * radius;
    }

    // ghost pool
    virtual bool GhostsEnabled() const { return false; }

    // origin point used to aim ghosts toward each lane end
    // override per lane for separate pools
    virtual void SetGhostPathOrigin(InputLane, float& origin_x, float& origin_y) const
    {
        SetReticleCenter(origin_x, origin_y);
    }

    // how far away from the lane end a ghost starts
    virtual float GetGhostTravelDistance() const { return GetLaneDepthPixels() * 0.65f; }

    virtual void DrawGhost(const float position_x, const float position_y, const InputLane lane, float depth_normalized, const float alpha, const float size) const
    {
        GameUI::Colour base = GetLaneColour(lane);
        GameUI::Colour ghost_color = { base.red * alpha, base.green * alpha, base.blue * alpha };
        GameUI::DrawFilledCircle(position_x, position_y, size, ghost_color, 12);
    }
};

