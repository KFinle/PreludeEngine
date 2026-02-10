#include "MusicTransport.h"
#include "Math/MathUtils.h"
#include <cmath>

void MusicTransport::Reset()
{
    raw_seconds = 0.0f;
    visual_seconds = 0.0f;
}

void MusicTransport::Update(const float dt_sec)
{
    raw_seconds += dt_sec;
    this->dt_seconds = dt_sec;

    // latency-compensated visual time
    visual_seconds = ClampFloat(raw_seconds - audio_latency_seconds, 0.0f, 999999.0f);
}

float MusicTransport::GetBeat() const
{
    return SecondsToBeats(visual_seconds, bpm);
}

float MusicTransport::GetBeatInBar() const
{
    return std::fmod(GetBeat(), static_cast<float>(beats_per_bar));
}

float MusicTransport::GetBarProgress() const
{
    return GetBeatInBar() / static_cast<float>(beats_per_bar);
}

int MusicTransport::GetBarIndex() const
{
    return static_cast<int>(GetBeat() / static_cast<float>(beats_per_bar));
}
