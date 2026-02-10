#pragma once
#include <vector>
#include "PatternLayer.h"

///////////////////
// Chord Pattern //
//////////////////////////////////////////////////
// Used for emitting a given chord rhythmically //
//////////////////////////////////////////////////
struct ChordPattern
{
    std::string pattern;
    int bars = 1;
    float step_beats = 0.25f;
    std::vector<NoteSpec> chord;

    // emit chord pattern
    void EmitAtBar(SequenceBuilder& b, const int start_bar) const
    {
        for (const NoteSpec& n : chord)
        {
            PatternLayer layer;
            layer.pattern = pattern;
            layer.bars = bars;
            layer.step_beats = step_beats;
            layer.note = n;
            layer.EmitAtBar(b, start_bar);
        }
    }
};
