#pragma once
#include "MusicalContext.h"
//////////////
// NoteSpec //
//////////////////////////////////////////////////////////////////////
// All the information needed to generate a note via the Oscillator //
//////////////////////////////////////////////////////////////////////
enum class Pitch
{
    C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B
};

enum class Articulation
{
    Sustained,
    Percussive
};

enum class SignalSource
{
    Sine,
    WhiteNoise
};

enum class VoiceType
{
    Kick,
    Snare,
    Hat,
    Lead, 
    Triangle, 
    Chord
};

inline bool IsDrumVoice(const VoiceType voice)
{
    return voice == VoiceType::Kick ||
           voice == VoiceType::Snare ||
           voice == VoiceType::Hat;
}

inline int GetDrumLaneIndex(const VoiceType voice)
{
    switch (voice)
    {
        case VoiceType::Kick:  return 0;
        case VoiceType::Snare: return 1;
        case VoiceType::Hat:   return 2;
        default:               return 0;
    }
}

struct NoteSpec
{
    Pitch pitch = Pitch::C;
    int octave = 4;
    float duration_beats = 1.0f;
    float velocity = 1.0f;
    float gate = 1.0f;
    float slide_to_hz = 0.0f;
    Articulation articulation = Articulation::Sustained;
    SignalSource signal_source = SignalSource::Sine;
    VoiceType voice = VoiceType::Lead;
};

enum class NoteDuration
{
    Whole,
    Half,
    Quarter,
    Eighth,
    Sixteenth,
    ThirtySecond
};

inline float DurationToBeats(const NoteDuration d, const MusicalContext& ctx)
{
    switch (d)
    {
        // TODO:
        // "whole" is one full bar, not 4 beats. Purely for convenience in alt timings
        // This is probably fine for now, but it's kind of for any sound designer/musician.
        // I should fix this at some point
        case NoteDuration::Whole: return static_cast<float>(ctx.beats_per_bar);
        case NoteDuration::Half: return 2.0f;
        case NoteDuration::Quarter: return 1.0f;
        case NoteDuration::Eighth: return 0.5f;
        case NoteDuration::Sixteenth: return 0.25f;
        case NoteDuration::ThirtySecond: return 0.125f;
        default: return 1.0f;
    }
}
