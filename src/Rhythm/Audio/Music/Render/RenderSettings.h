#pragma once

/////////////////////
// RENDER SETTINGS //
/////////////////////
struct RenderSettings
{
    int sample_rate = 48000;
    float tail_seconds = 0.25f; 

    // default synth voice params
    float attack_seconds = 0.01f;
    float decay_seconds = 0.10f;
    float sustain = 0.80f;
    float release_seconds = 0.20f;

    // percussive defaults
    float percussion_attack_seconds = 0.0f;
    float percussion_decay_seconds = 0.12f;
    float percussion_sustain = 0.0f;
    float percussion_release_seconds = 0.0f;
    float slide_time_seconds = 0.4f;
    
};
