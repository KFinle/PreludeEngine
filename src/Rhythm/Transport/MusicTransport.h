#pragma once

struct MusicTransport
{
    // raw & visual time
    float raw_seconds = 0.0f;
    float visual_seconds = 0.0f;
    float dt_seconds  = 0.0f;

    // musical context
    float bpm = 120.0f;
    int beats_per_bar = 4;

    // ~30ms feels about right to me; other systems may vary
    // if there's time, I should consider making a settings menu 
    // for the player to adjust their own latency
    float audio_latency_seconds = 0.030f;

    void Reset();
    void Update(float dt_sec);

    // getters
    float GetBeat() const;
    float GetBeatInBar() const;
    float GetBarProgress() const;
    int GetBarIndex() const;
};
