#pragma once

#include <cstdint>

#include "Math/MathUtils.h"

namespace Rhythm
{
    inline uint32_t BeatBucket(const float beat, const float bucket_scale)
    {
        const float clamped_beat = MaxFloat(0.0f, beat);
        return static_cast<uint32_t>(floorf(clamped_beat * bucket_scale + 0.5f));
    }

    // basically a Murmurhash with golden ratio multiplication
    inline uint32_t HashBucket(const uint32_t bucket)
    {
        // multiply
        uint32_t hash_value = bucket * 2654435761u;
        // xor
        hash_value ^= hash_value >> 16;
        // multiply
        hash_value *= 2246822519u;
        // xor
        hash_value ^= hash_value >> 13;
        return hash_value;
    }
}
