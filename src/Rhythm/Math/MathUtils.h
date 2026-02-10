#pragma once
#include <cmath>

#include "RhythmSettings.h"
//////////////////////////////////////
// just a few common math utilities //
//////////////////////////////////////

inline float two_pi = Rhythm::kTwoPi;
inline float PI = 3.141592653;

inline float ClampFloat(const float value, const float min_value, const float max_value)
{
    return (value < min_value) ? min_value : (value > max_value) ? max_value : value;
}

inline float MinFloat(const float left, const float right)
{
    return (left < right) ? left : right;
}

inline float MaxFloat(const float left, const float right)
{
    return (left > right) ? left : right;
}

inline int IntMax(const int left, const int right)
{
    return (left > right) ? left : right;
}


inline float LerpFloat(const float from_value, const float to_value, float time)
{
    time = ClampFloat(time, 0.0f, 1.0f);
    return from_value + (to_value - from_value) * time;
}

inline float SecondsToBeats(const float seconds, const float bpm)
{
    return seconds * (MaxFloat(1.0f, bpm) / 60.0f);
}

inline float BeatsToSeconds(const float beats, const float bpm)
{
    return (beats * 60.0f) / MaxFloat(1.0f, bpm);
}

inline float WrapPhase(float phase)
{
    phase = phase - floorf(phase);
    if (phase < 0.0f) phase += 1.0f;
    return phase;
}

inline float SmoothStep(float t_value)
{
    t_value = ClampFloat(t_value, 0.0f, 1.0f);
    return t_value * t_value * (3.0f - 2.0f * t_value);
}
