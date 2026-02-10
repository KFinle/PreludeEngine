#pragma once
#include "Audio/Music/Events/NoteEvent.h"

//////////////////
// MIX SETTINGS //
///////////////////////////////////////////////////////////////////////////////
// Use this to adjust gain of voices independently of their default levels.  //
// This is ideal for changing the mix on specific songs                      //
///////////////////////////////////////////////////////////////////////////////
struct MixSettings
{
    // global
    float master_gain = 0.1f;

    // drums
    float kick_gain = 1.0f;
    float snare_gain = 0.8f;
    float hat_gain = 0.9f;

    // pitched voices
    float lead_gain = 0.10f; 
    float triangle_gain = 0.5f; 
    float chord_gain = 0.1f;

    // gain lookup
    float GetVoiceGain(const VoiceType voice) const
    {
        switch (voice)
        {
            case VoiceType::Kick: return kick_gain;
            case VoiceType::Snare: return snare_gain;
            case VoiceType::Hat: return hat_gain;
            case VoiceType::Lead: return lead_gain;
            case VoiceType::Triangle: return triangle_gain;
            case VoiceType::Chord: return chord_gain;
        }

        return 1.0f;
    }
};
