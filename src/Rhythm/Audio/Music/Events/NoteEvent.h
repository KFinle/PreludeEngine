#pragma once

#include "Audio/Music/Orchestration/NoteSpec.h"

///////////////
// NoteEvent //
/////////////////////////////////////////////////////////////////////
// defines the parameters for any musical notes to be rendered out //
/////////////////////////////////////////////////////////////////////
struct NoteEvent
{
    // musical time
    float start_beat = 0.0f;
    float duration_beat = 1.0f;

    // time in seconds
    float start_sec = 0.0f;
    float duration_sec = 0.0f;

    // pitch & dynamics
    int midi_note = 60;
    float velocity = 1.0f;
    int channel = 0;
    float slide_to_hz = 0.0f;

    // articulation
    Articulation articulation = Articulation::Sustained;
    SignalSource signal_source = SignalSource::Sine;
    VoiceType voice = VoiceType::Lead;
};
