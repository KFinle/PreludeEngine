#pragma once

#include <cstdlib>

namespace Rhythm
{
    inline constexpr float kPi = 3.14159265359f;
    inline constexpr float kTwoPi = 2.0f * kPi;

    inline float RandFloat01()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    inline float RandFloatRange(const float min_value, const float max_value)
    {
        return RandFloat01() * (max_value - min_value) + min_value;
    }
}
