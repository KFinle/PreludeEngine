#pragma once
#include "PatternLayer.h"
#include "Audio/Music/Instruments/DrumKit.h"
////////////////////////////////////
// Emit kick, Snare, Hat patterns //
////////////////////////////////////
struct KickPattern : PatternLayer
{
    KickPattern(const std::string& kick_pattern, const int kick_bars)
    {
        pattern = kick_pattern;
        bars = kick_bars;
        step_beats = 0.25f;
        note = DrumKit::Kick();
    }
};

struct SnarePattern : PatternLayer
{
    SnarePattern(const std::string& snare_pattern, const int snare_bars)
    {
        pattern = snare_pattern;
        bars = snare_bars;
        step_beats = 0.25f;
        note = DrumKit::Snare();
    }
};

struct HatPattern : PatternLayer
{
    HatPattern(const std::string& hat_pattern, const int hat_bars)
    {
        pattern = hat_pattern;
        bars = hat_bars;
        step_beats = 0.25f;
        note = DrumKit::Hat();
    }
};
