# HUD Skinning Guide

This guide describes how to add and tune HUD skins in the UI layer.
Skins are visual only and should not modify gameplay behavior.

## Where Skins Live
- Implement new skins as header-only classes in `src/Game/UI/HUD/Skins/`.
- Follow existing naming: `HUDSkinYourName.h`.
- Wire the skin into the HUD system (enum + factory) as described below.

## Core Interface
All skins implement `IHUDSkin` in `src/Game/UI/HUD/HUDSkin.h`.

Required methods:
- `BeginFrame(screen_w, screen_h, current_beat, dt_sec)`
- `DrawBackground()`
- `DrawLanes()`
- `DrawEntity(const HUDSkinEntity& entity)`
- `DrawReticle()`
- `GetLaneColor(InputLane lane) const`

Optional overrides:
- `GetLaneDepthPixels() const`
- `GetActiveLanesMask() const`
- `GetLaneTarget(InputLane lane, float& tx, float& ty) const`
- `GetReticleCenter(float& cx, float& cy) const`

## Lane Targets (Non-Center Hit Points)
To move the hit target away from the center reticle, override `GetLaneTarget`.
This lets you place targets on a finish line, a corner apex, or a side gate.

Example pattern (inside a skin):
```
void GetLaneTarget(InputLane lane, float& tx, float& ty) const override
{
    tx = lane_x;
    ty = hit_y;
}
```

Notes still use `entity.depth_normalized` from 0..1, so your `DrawEntity` should lerp
between a spawn position and the lane target if you want custom trajectories.

## Active Lanes (Limited Buttons)
Override `GetActiveLanesMask()` to disable lanes for a skin.
Bits map to lanes: Up=1<<0, Right=1<<1, Down=1<<2, Left=1<<3.

Example:
```
uint8_t GetActiveLanesMask() const override
{
    return (1 << 1) | (1 << 3); // Right + Left only
}
```

## Drawing Entities
Each note is provided as a `HUDSkinEntity`:
- `lane`: which lane it belongs to
- `depth_normalized`: 0 at hit target, 1 at spawn distance
- `magnitude`: note emphasis
- `consumed`: already hit/missed
- `id`: stable identifier (useful for skin-only caching)

Common patterns:
- Straight lane: `pos = center + dir * lane_depth * depth_normalized`
- Custom target: `pos = lerp(target, spawn, depth_normalized)`

## Wiring a New Skin
1) Add your header in `src/Game/UI/HUD/Skins/`.
2) Add an enum entry in `src/Gameplay/HUDMode.h`.
3) Include the header and add a case in `GetSkinForMode` in
   `src/Game/UI/HUD/GameplayHUD.cpp`.
4) Add the name in `HudModeName` in `src/Game/UI/HUD/GameplayHUD.cpp`.
5) Add the mode to the initialization list and punishable list.

## Tips
- Keep all visuals in HUD code only.
- Avoid heavy per-frame allocations; prefer local computations.
- If you cache data, key off `HUDSkinEntity::id` and remove it when consumed.
