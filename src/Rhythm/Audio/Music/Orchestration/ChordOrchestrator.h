#pragma once

#include "SequenceBuilder.h"
#include "NoteSpec.h"
#include "ChordSequence.h"

////////////////////////
// Chord Orchestrator //
////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is mostly unused now because the ChordPattern file takes care of most use-cases.              //
// it does still provide a flexible way to play with additional chord timings/articulations though    //
// I have demonstrated its use in MakeSnog_Medium/Brutal (mostly because rewriting the songs with the //
// ChordPattern setup would take too much time                                                        // 
////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace ChordOrchestrator
{
    // play chord over bars
    inline void Pad(SequenceBuilder& sequence_builder, const int start_bar, const int bars, const std::vector<NoteSpec>& chord)
    {
        ChordSequence::EmitBars(sequence_builder, start_bar, bars, chord);
    }

    // play short stabs at start of each bar
    inline void Stab(SequenceBuilder& sequence_builder, const int start_bar, const int bars, const std::vector<NoteSpec>& chord, const float stab_beats = 0.5f)
    {
        const MusicalContext& context = sequence_builder.GetContext();
        const float start_beat = start_bar * context.beats_per_bar;

        for (int bar = 0; bar < bars; ++bar)
        {
            for (const NoteSpec& chord_note : chord)
            {
                NoteSpec note = chord_note;
                note.duration_beats = stab_beats;

                sequence_builder.Add(start_beat + bar * context.beats_per_bar, note);
            }
        }
    }

    // play chord for every step in the bar
    inline void Rhythm(SequenceBuilder& sequence_builder, const int start_bar, const int bars, const std::vector<NoteSpec>& chord, const float step_beats = 1.0f)
    {
        const MusicalContext& context = sequence_builder.GetContext();
        const float bar_beats = context.beats_per_bar;

        for (int bar = 0; bar < bars; ++bar)
        {
            const float bar_start = (start_bar + bar) * bar_beats;

            for (float step_offset = 0.0f; step_offset < bar_beats; step_offset += step_beats)
            {
                for (const NoteSpec& chord_note : chord)
                {
                    NoteSpec note = chord_note;
                    note.duration_beats = step_beats * 0.9f;

                    sequence_builder.Add(bar_start + step_offset, note);
                }
            }
        }
    }
}
