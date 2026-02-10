#pragma once
#include "cmath"
#include "Audio/Music/Orchestration/NoteSpec.h"

// This is a small collection of methods for converting
// pitch tokens into MIDI pitches or frequencies.

///////////////////////////////////////////////////////////////////////////////
// TODO:
// Maybe implement a MIDI reader to allow
// the player or devs to import a MIDI file?
// Might be excessive for the sake of this project, but it would be cool 
///////////////////////////////////////////////////////////////////////////////

inline int PitchToMidi(Pitch p, const int octave)
{
    // MIDI: C-1 = 0, C4 = 60
    return (octave + 1) * 12 + static_cast<int>(p);
}

inline double MidiToFrequency(const int midi)
{
    // 69 == A4 == 440Hz == standard tuning for modern music 
    return 440.0 * std::pow(2.0, (midi - 69) / 12.0);
}

inline Pitch MidiToPitchClass(const int midi)
{
    switch (midi % 12)
    {
        case 0: return Pitch::C;
        case 1: return Pitch::Cs;
        case 2: return Pitch::D;
        case 3: return Pitch::Ds;
        case 4: return Pitch::E;
        case 5: return Pitch::F;
        case 6: return Pitch::Fs;
        case 7: return Pitch::G;
        case 8: return Pitch::Gs;
        case 9: return Pitch::A;
        case 10: return Pitch::As;
        case 11: return Pitch::B;
        default: return Pitch::C;
    }
}
