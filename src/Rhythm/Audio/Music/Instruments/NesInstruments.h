#pragma once
#include "Audio/Music/Orchestration/NoteSpec.h"

////////////////////
// NESInstruments //
////////////////////
// TODO:
// Make a real pulse synth                 //
// This is currently just a square wave    //
// Pulse should have timing variations     //
// Currently, this isn't REALLY the sound  //
// of a NES :(                             //
/////////////////////////////////////////////
namespace Nes
{
    inline NoteSpec PulseLead()
    {
        NoteSpec note;
        note.pitch = Pitch::C;
        note.octave = 4;
        note.duration_beats = 0.25f;
        note.velocity = 0.8f;
        note.articulation = Articulation::Sustained;
        note.signal_source = SignalSource::Sine;
        note.voice = VoiceType::Lead;
        return note;
    }

    inline NoteSpec TriangleBass()
    {
        NoteSpec note;
        note.pitch = Pitch::C;
        note.octave = 2;
        note.duration_beats = 0.5f;
        note.velocity = 0.9f;
        note.articulation = Articulation::Sustained;
        note.signal_source = SignalSource::Sine;
        note.voice = VoiceType::Triangle;
        return note;
    }
}
