#pragma once
#include <vector>
#include "SequenceBuilder.h"
#include "NoteSpec.h"

namespace ChordSequence
{
    // emit chord for given number of bars
    inline void EmitBars(SequenceBuilder& b, const int start_bar, const int bars, const std::vector<NoteSpec>& chord)
    {
        if (chord.empty() || bars <= 0) return;

        const MusicalContext& context = b.GetContext();

        const float start_beat = static_cast<float>(start_bar) * context.beats_per_bar;

        const float duration_beats = static_cast<float>(bars) * context.beats_per_bar;

        for (const NoteSpec& n : chord)
        {
            NoteSpec note = n;
            note.duration_beats = duration_beats;
            b.Add(start_beat, note);
        }
    }
}
