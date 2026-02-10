#include <cmath>
#include "../Primitives/Oscillator.h"

////////////////
// Kick Synth //
/////////////////////////////////////////////////////////
// builds a kick sound using a basic sine wave         //
// Note: NES actually uses a triangle wave, but        //
//       using a sine wave feels a bit more clear      //
//       without taking away from the retro audio feel //
/////////////////////////////////////////////////////////
namespace KickSynth
{
    inline void Render(std::vector<float>& out, Oscillator& oscillator, const int samples,
                       const float sample_rate, const float base_frequency_hz, const float start_freq_hz,
                       float sweep_seconds, float amp_decay_seconds)
    {
        if (samples <= 0) return;

        // Safety clamps
        if (sweep_seconds < 0.001f) sweep_seconds = 0.001f;
        if (amp_decay_seconds < 0.001f) amp_decay_seconds = 0.001f;

        const float sweep_coefficient = std::exp(-1.0f / (sweep_seconds * sample_rate));

        const float amplitude_coefficient = std::exp(-1.0f / (amp_decay_seconds * sample_rate));

        float frequency = start_freq_hz;
        float amplitude = 1.0f;

        for (int sample_index = 0; sample_index < samples; ++sample_index)
        {
            frequency = base_frequency_hz + (frequency - base_frequency_hz) * sweep_coefficient;
            oscillator.SetFrequency(frequency);

            const float sample_value = oscillator.GetNextSample();
            out.push_back(sample_value * amplitude);

            amplitude *= amplitude_coefficient;
        }
    }
}
