#pragma once
#include <vector>

#include "RhythmSettings.h"

// TODO:
// Might be worth adding a proper compressor, but this should be enough for now.
// Should also consider adding a simple ducker for the triangle synth.
// May also be worth adding a sidechain ducking setup for the kick drums.

/////////////////////////
// Transient Utilities //
///////////////////////////////////////////////////////
// Tools used for affecting the start of a NoteEvent //
///////////////////////////////////////////////////////
namespace TransientUtils
{
    
    static float WhiteNoise()
    {
        return 2.0f * Rhythm::RandFloat01() - 1.0f;
    }


    // use some whitenoise to simulate a bit of transient click
    static void AddClick(std::vector<float>& buffer, const float amplitude, const int length_samples)
    {
        const int buffer_size = static_cast<int>(buffer.size());
        const int click_length = (length_samples < buffer_size) ? length_samples : buffer_size;

        for (int sample_index = 0; sample_index < click_length; ++sample_index)
        {
            float fade_t = 1.0f - static_cast<float>(sample_index) / static_cast<float>(click_length);
            buffer[sample_index] += WhiteNoise() * amplitude * fade_t;
        }
    }
}
