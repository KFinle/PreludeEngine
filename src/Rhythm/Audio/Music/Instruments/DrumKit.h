#pragma once
#include "Audio/Music/Orchestration/NoteSpec.h"

/////////////
// DrumKit //
/////////////
// TODO:
// Maybe allow for some more sounds? //
// Other cymbals would be nice.      //
// Toms, open hi-hat, cross-stick?   //
///////////////////////////////////////
namespace DrumKit
{
    inline NoteSpec Kick()
    {
        NoteSpec note;
        note.pitch = Pitch::C;
        note.octave = 2;
        note.duration_beats = 0.25f;
        note.velocity = 1.5f;
        note.articulation = Articulation::Percussive;
        note.signal_source = SignalSource::Sine;
        note.voice = VoiceType::Kick;
        
        return note;
    }

    inline NoteSpec Snare()
    {
        NoteSpec note;
        note.pitch = Pitch::D;
        note.octave = 3;
        note.duration_beats = 0.25f;
        note.velocity = 0.85f;
        note.articulation = Articulation::Percussive;
        note.signal_source = SignalSource::WhiteNoise;
        note.voice = VoiceType::Snare;

        return note;
    }

    inline NoteSpec Hat()
    {
        NoteSpec note;
        note.pitch = Pitch::F;
        note.octave = 4;
        note.duration_beats = 0.005f;
        note.velocity = 0.5f;
        note.articulation = Articulation::Percussive;
        note.signal_source = SignalSource::WhiteNoise;
        note.voice = VoiceType::Hat;

        return note;
    }
}
