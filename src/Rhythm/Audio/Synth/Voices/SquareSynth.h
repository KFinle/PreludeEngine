#pragma once
#include <vector>
#include "Audio/Synth/Primitives/Oscillator.h"
#include "Audio/Synth/Primitives/EnvelopeFilter.h"

//////////////////
// Square Synth //
////////////////////////////////////////////////////////////////////////////////
// Used to generate square wave tones.                                        //
// NOTE: this is also what I currently used to generate saw and sine tones.   //
//       That should of course be changed, but for the sake of avoiding extra //
//       files and a refactor, I am leaving it as-is for now.                 //
////////////////////////////////////////////////////////////////////////////////

namespace SquareSynth
{
    // square wave
    // very fast attack
    // short decay
    void Render(std::vector<float>& out, Oscillator& oscillator, EnvelopeFilter& Envelope, int samples, float frequency_hz, float sample_rate);
}

namespace SquareSynth
{
    inline void Render(std::vector<float>& out, Oscillator& oscillator, const WaveType wavetype, EnvelopeFilter& envelope, const int samples, const float frequency_hz, const float sample_rate)
    {
        if (samples <= 0) return;

        oscillator.SetWaveType(wavetype);
        oscillator.SetFrequency(frequency_hz);

        envelope.SetSampleRate(sample_rate);
        envelope.SetAttack(0.001f);
        envelope.SetDecay(0.05f);
        envelope.SetSustain(0.7f);
        envelope.SetRelease(0.02f);
        envelope.NoteOn();

        for (int sample_index = 0; sample_index < samples; ++sample_index)
        {
            float sample_value = oscillator.GetNextSample();
            float env_value = envelope.GetNextSample();
            out.push_back(sample_value * env_value);
        }
    }

    inline void RenderWithSlide(std::vector<float>& out, Oscillator& oscillator, EnvelopeFilter& envelope, const int samples, const float start_hz, const float end_hz, const float sample_rate, const float slide_time_seconds)
    {
        if (samples <= 0) return;

        oscillator.SetWaveType(Square);

        envelope.SetSampleRate(sample_rate);
        envelope.SetAttack(0.001f);
        envelope.SetDecay(0.05f);
        envelope.SetSustain(0.7f);
        envelope.SetRelease(0.02f);
        envelope.NoteOn();

        const int slide_samples = static_cast<int>(std::round(slide_time_seconds * sample_rate));
        const int clamped_slide_samples = std::max(1, std::min(slide_samples, samples));

        for (int sample_index = 0; sample_index < samples; ++sample_index)
        {
            float slide = 1.0f;

            if (sample_index < clamped_slide_samples)
            {
                slide = static_cast<float>(sample_index) / static_cast<float>(clamped_slide_samples);
            }

            float current_hz = start_hz * std::pow(end_hz / start_hz, slide);
            oscillator.SetFrequency(current_hz);
            out.push_back(oscillator.GetNextSample() * envelope.GetNextSample());
        }
    }
}
