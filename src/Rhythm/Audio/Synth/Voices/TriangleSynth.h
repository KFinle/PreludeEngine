#pragma once
#include <vector>
#include "Audio/Synth/Primitives/Oscillator.h"

////////////////////
// Triangle Synth //
/////////////////////////////////////////////////////////////////////////////////////////////////
// Generates a triangle wave. Ideally, this should be combined with SquareSynth.h to make a    //
// unified synth interface, but the system architecture was not fully fleshed out at the time. //
/////////////////////////////////////////////////////////////////////////////////////////////////
namespace TriangleSynth
{
    // triangle bass:
    void Render(std::vector<float>& out, Oscillator& oscillator, int samples, float frequency_hz);
}

namespace TriangleSynth
{
    inline void Render(std::vector<float>& out, Oscillator& oscillator, const int samples, const float frequency_hz) 
    {
        if (samples <= 0) return;

        oscillator.SetWaveType(Triangle);
        oscillator.SetFrequency(frequency_hz);

        for (int sample_index = 0; sample_index < samples; ++sample_index)
        {
            out.push_back(oscillator.GetNextSample());
        }
    }
}
