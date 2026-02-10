#pragma once
#include <vector>

// TODO:
// If relevant, I should expand this to include some more tools
// Consider fadeout, crossfade, gain staging, etc.
// Keep this focused unless a real need shows up.

//////////////////////
// Buffer Utilities //
////////////////////////////////////////////////////////////////////////////
// Tools used for processing a mix buffer prior to rendering out to audio //
////////////////////////////////////////////////////////////////////////////
namespace BufferUtils
{
    // apply a fadeout to the buffer to minimize overlapping clicks
    static void ApplyFadeOut(std::vector<float>& buffer, const int fade_samples)
    {
        const int buffer_size = static_cast<int>(buffer.size());
        if (fade_samples <= 1 || buffer_size <= fade_samples) return;

        const float denom = static_cast<float>(fade_samples - 1);

        for (int fade_index = 0; fade_index < fade_samples; ++fade_index)
        {
            float fade_t = 1.0f - (static_cast<float>(fade_index) / denom);
            buffer[buffer_size - fade_samples + fade_index] *= fade_t;
        }
    }
}
