#pragma once
#include <vector>

#include "MusicalContext.h"
#include "NoteSpec.h"
#include "Audio/Music/Events/EventSequence.h"
#include "Audio/Music/Events/NoteBuilder.h"
#include "Audio/Music/Render/MixSettings.h"

/////////////////////////////
// Builds an EventSequence //
/////////////////////////////
class SequenceBuilder
{
public:
    explicit SequenceBuilder(const MusicalContext context)
    {
        m_context = context;
    }

    const MusicalContext& GetContext() const { return m_context; }

    // add single note at beat position
    SequenceBuilder& Add(const float start_beat, const NoteSpec& note)
    {
        m_events.push_back(NoteBuilder::Build(note, start_beat));
        return *this;
    }

    // add chord at beat position
    SequenceBuilder& AddChord(const float start_beat, const std::vector<NoteSpec>& notes)
    {
        for (const auto& note : notes)
        {
            Add(start_beat, note);
        }
        return *this;
    }

    // finalize sequence
    EventSequence Build(const MixSettings& mix_settings = MixSettings{}) const
    {
        EventSequence sequence;
        sequence.bpm = m_context.bpm;
        sequence.beats_per_bar = m_context.beats_per_bar;
        sequence.notes = m_events;
        sequence.mix = mix_settings;

        sequence.BakeSeconds();
        sequence.SortByStart();

        return sequence;
    }

private:
    MusicalContext m_context;
    std::vector<NoteEvent> m_events;
};
