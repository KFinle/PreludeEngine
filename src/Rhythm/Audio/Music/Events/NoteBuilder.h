#pragma once
#include "NoteEvent.h"
#include "Midi.h"
#include "Audio/Music/Orchestration/NoteSpec.h"

/////////////////
// NoteBuilder //
/////////////////////////////////////////
// convert a NoteSpec into a NoteEvent //
/////////////////////////////////////////
struct NoteBuilder
{
    static NoteEvent Build(const NoteSpec& spec, const float start_beat)
    {
        NoteEvent event;

        // musical time
        event.start_beat = start_beat;
        event.duration_beat = spec.duration_beats * spec.gate;

        // pitch & dynamics
        event.midi_note = PitchToMidi(spec.pitch, spec.octave);
        event.velocity = spec.velocity;
        event.channel = 0;

        // articulation
        event.articulation = spec.articulation;
        event.signal_source = spec.signal_source;
        event.voice = spec.voice;
        event.slide_to_hz = spec.slide_to_hz;
        
        return event;
    }
};
