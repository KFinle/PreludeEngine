#pragma once

#include <algorithm>
#include <vector>
#include "NoteEvent.h"
#include "Audio/Music/Render/MixSettings.h"

////////////////////
// Event Sequence //
///////////////////////////////////////////////////////////////
// Collection of NoteEvents representing a musical sequence. //
// This is used for both audio rendering and gameplay events //
///////////////////////////////////////////////////////////////
struct EventSequence
{
    float bpm = 120.0f;
    int beats_per_bar = 4;
    MixSettings mix;
    std::vector<NoteEvent> notes;

    void SortByStart()
    {
        std::sort(notes.begin(), notes.end(), CompareNotes);
    }

    static bool CompareNotes(const NoteEvent& a, const NoteEvent& b)
    {
        return (a.start_beat != b.start_beat) ? a.start_beat < b.start_beat : a.midi_note < b.midi_note;
    }

    float GetLengthBeats() const
    {
        if (notes.empty()) return 0.0f;

        float max_end = 0.0f;
        for (const auto& note : notes)
        {
            max_end = std::max(max_end, note.start_beat + note.duration_beat);
        }
        
        return max_end;
    }

    float GetLengthSec() const
    {
        return BeatsToSeconds(GetLengthBeats());
    }

    void BakeSeconds()
    {
        for (auto& note : notes)
        {
            note.start_sec = BeatsToSeconds(note.start_beat);
            note.duration_sec = BeatsToSeconds(note.duration_beat);
        }
    }

private:
    float BeatsToSeconds(const float beats) const
    {
        return beats * (60.0f / bpm);
    }
};
