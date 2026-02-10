#pragma once

#include <cstdint>

#include <vector>
#include <algorithm>
#include "Audio/Music/Orchestration/NoteSpec.h"
#include "Gameplay/Lanes.h"
#include "Math/MathUtils.h"

///////////////
// Note Pool //
///////////////////////////////////////////////////
// These are the interactable notes that appear  //
// on Lanes. This file describes their behaviour //
// and structure.                                //
///////////////////////////////////////////////////
namespace GameplayPool
{
    ////////////////////
    // Note Structure //
    ////////////////////
    struct Note
    {
        uint32_t id = 0;
  
        // note data
        float beat = 0.0f;
        InputLane lane = InputLane::Up;
        VoiceType voice = VoiceType::Lead;
        float size = 1.0f;

        // motion data
        float distance_px = 0.0f;
        float pixels_per_second = 0.0f;

        // life data
        bool consumed = false;
        bool miss_registered = false;
    };

    
    ///////////////
    // Note Pool //
    ///////////////
    class NotePool
    {
    public:
        static constexpr float max_step_seconds = 0.1f;

        void Clear()
        {
            m_notes.clear();
        }

        // spawn a new entity with pre-computed motion parameters
        void SpawnNote(const float beat, const InputLane lane, const VoiceType voice, const float magnitude, const float initial_distance_px, const float px_per_second)
        {
            Note note{};
            note.beat = beat;
            note.lane = lane;
            note.voice = voice;
            note.size = magnitude;
            note.distance_px = initial_distance_px;
            note.pixels_per_second = px_per_second;
            note.consumed = false;
            note.id = static_cast<uint32_t>(m_notes.size()) + 1;

            m_notes.push_back(note);
        }

        // update motion for all non-consumed entities
        void StepMotion(float dt_sec, const float minimum_distance)
        {
            dt_sec = ClampFloat(dt_sec, 0.0f, max_step_seconds);

            for (auto& note : m_notes)
            {
                if (note.consumed) continue;

                note.distance_px -= note.pixels_per_second * dt_sec;
                if (note.distance_px < minimum_distance) note.distance_px = minimum_distance;
            }
        }

        // remove consumed and old entities
        void CullByBeat(const float current_beat, const float older_than_beats)
        {
            auto iterator = m_notes.begin();
            while (iterator != m_notes.end())
            {
                const bool dead_note = iterator->consumed;
                const bool old_note = (iterator->beat < current_beat - older_than_beats);

                if (dead_note || old_note) iterator = m_notes.erase(iterator);
                else ++iterator;
            }
        }

        size_t Count() const { return m_notes.size(); }

        Note& GetNote(const size_t note_index) { return m_notes[note_index]; }
        const Note& GetNote(const size_t note_index) const { return m_notes[note_index]; }

    private:
        std::vector<Note> m_notes;
    };
}
