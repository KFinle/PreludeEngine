#pragma once

#include <cstdint>

#include "RhythmSettings.h"
///////////
// Lanes //
/////////////////////////////////////////////////////////
// This file contains tools for retrieving information //
// about the lanes we place gameplay notes on          //
/////////////////////////////////////////////////////////

constexpr int InputLaneCount = 4;

// which direction are the notes coming from?
enum class InputLane
{
    Up,
    Right,
    Down,
    Left
};


// should probably move this to math, but it's mostly used for lanes anyway
// though, I should consider a general refactor to make my life easier 
struct Vector2
{
    float x_position = 0.0f;
    float y_position = 0.0f;
};

inline int GetLaneIndex(InputLane lane)
{
    return static_cast<int>(lane);
}

inline Vector2 GetLaneDirection(const InputLane lane)
{
    switch (lane)
    {
        case InputLane::Up: return {  0.0f,  1.0f };
        case InputLane::Right: return {  1.0f,  0.0f };
        case InputLane::Down: return {  0.0f, -1.0f };
        case InputLane::Left: return { -1.0f,  0.0f };
    }
    return {};
}

inline float GetLaneAngle(const InputLane lane)
{
    switch (lane)
    {
        case InputLane::Up: return Rhythm::kPi * 0.5f;
        case InputLane::Right: return 0.0f;
        case InputLane::Down: return Rhythm::kPi * 1.5f;
        case InputLane::Left: return Rhythm::kPi;
    }
    return 0.0f;
}

inline uint8_t GetLaneBit(const InputLane lane)
{
    switch (lane)
    {
        case InputLane::Up: return 1 << 0;
        case InputLane::Right: return 1 << 1;
        case InputLane::Down: return 1 << 2;
        case InputLane::Left: return 1 << 3;
        default: return 0;
    }
}

inline bool IsLaneActive(const uint8_t mask, const InputLane lane)
{
    return (mask & GetLaneBit(lane)) != 0;
}

inline int ActiveLaneCount(const uint8_t mask)
{
    int active_count = 0;
    for (int lane_index = 0; lane_index < 4; ++lane_index)
    {
        if (IsLaneActive(mask, static_cast<InputLane>(lane_index))) ++active_count;
    }
    return active_count;
}
