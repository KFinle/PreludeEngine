#pragma once
#include <algorithm>
#include <cmath>
#include <vector>

#include "RhythmSettings.h"

/////////////////
// Noise Synth //
//////////////////////////////////////////////////////////////////////////
// Uses white noise to shape the sounds of our snare and hi-hats.       //
// Ideally, I should add pink noise to this as well, but time is tight. //
//////////////////////////////////////////////////////////////////////////

static float WhiteNoise()
{
    return 2.0f * Rhythm::RandFloat01() - 1.0f;
}

struct HighPassFilter
{
    float coefficient = 0.0f;
    float previous_output = 0.0f;

    void SetCutoff(const float cutoff_hz, const float sample_rate)
    {
        const float cutoff_coefficient = std::exp(-2.0f * Rhythm::kPi * cutoff_hz / sample_rate);
        coefficient = cutoff_coefficient;
    }

    float Process(const float input_sample)
    {
        float output_sample = input_sample - previous_output + coefficient * previous_output;
        previous_output = output_sample;
        return output_sample;
    }
};


namespace NoiseSynth
{
    enum class NoiseType
    {
        Snare,
        Hat
    };
    void Render(std::vector<float>& out, int samples, float attack_seconds, float decay_seconds, float sample_rate, NoiseType type);
}


namespace NoiseSynth
{
    inline void Render(std::vector<float>& out, int samples, float attack_seconds, float decay_seconds, const float sample_rate, const NoiseType type)
    {
        if (samples <= 0) return;

        // clamp attack / decay
        attack_seconds = std::max(attack_seconds, 0.001f);
        decay_seconds = std::max(decay_seconds, 0.001f);

        // hi-hat tweaks
        if (type == NoiseType::Hat)
        {
            const int max_hat_samples = static_cast<int>(0.03f * sample_rate);
            samples = std::min(samples, max_hat_samples);
            decay_seconds *= 0.15f;
        }

        // envelope setup
        const int attack_samples = static_cast<int>(attack_seconds * sample_rate);
        const float decay_coefficient = std::exp(-1.0f / (decay_seconds * sample_rate));
        float amplitude = 1.0f;

        // hat filter
        HighPassFilter high_pass_filter;
        if (type == NoiseType::Hat)  high_pass_filter.SetCutoff(6000.0f, sample_rate);

        // render loop
        for (int sample_index = 0; sample_index < samples; ++sample_index)
        {
            float env;
            if (sample_index < attack_samples) env = static_cast<float>(sample_index) / static_cast<float>(attack_samples);
            else  env = amplitude;
            float noise_value = WhiteNoise();
            if (type == NoiseType::Hat) noise_value = high_pass_filter.Process(noise_value);
            out.push_back(noise_value * env);
            amplitude *= decay_coefficient;
        }
    }
}
