#pragma once

#include "MelodyBuilder.h"
#include "Audio/Music/Instruments/NesInstruments.h"

// TODO:
// intent: NES-style melodies
// reality: close enough, but the leads aren't real pulse. I should fix this later

namespace NesMelody
{
    // main lead synth 
    inline void Pulse(SequenceBuilder& builder, const std::string& pattern, const int bars, const float step_beats = 0.25f, const float start_beat = 0.0f)
    {
        NoteSpec base = Nes::PulseLead();

        MelodyBuilder::Settings settings;
        settings.bars = bars;
        settings.step_beats = step_beats;
        settings.start_beat = start_beat;

        MelodyBuilder::EmitMonophonic(builder, base, pattern, settings);
    }

    // triangle bass
    inline void Triangle(SequenceBuilder& builder, const std::string& pattern, const int bars, const float step_beats = 1.0f, const float start_beat = 0.0f)
    {
        NoteSpec base = Nes::TriangleBass();

        MelodyBuilder::Settings settings;
        settings.bars = bars;
        settings.step_beats = step_beats;
        settings.start_beat = start_beat;

        MelodyBuilder::EmitMonophonic(builder, base, pattern, settings);
    }
}
