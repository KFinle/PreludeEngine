#pragma once

#include <cstdint>

#include "Gameplay/Lanes.h"

#include "Util/BeatHash.h"

//////////////////////
// Timing Utilities //
///////////////////////////////////////////////////////////////
// Used for organizing the placement and spawn time of notes //
// NOTE: Was originally just randomizing which lane was      //
//       used for each note, but adding the visual pools     //
//       required more deterministic placements.             //
///////////////////////////////////////////////////////////////

inline InputLane GetLaneForBeat(const float beat, const uint8_t mask, const float bucket_scale)
{
    InputLane lanes[4] =
    {
        InputLane::Up, InputLane::Right, InputLane::Down, InputLane::Left
    };

    InputLane active_lanes[4];
    int active_count = 0;
    for (const auto& lane : lanes)
    {
        if (IsLaneActive(mask, lane)) active_lanes[active_count++] = lane;
    }

    if (active_count == 0) return InputLane::Up;

    const uint32_t bucket = Rhythm::BeatBucket(beat, bucket_scale);
    const uint32_t hashed = Rhythm::HashBucket(bucket);
    return active_lanes[hashed % static_cast<uint32_t>(active_count)];
}
