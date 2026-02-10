#pragma once
#include <string>
#include "SequenceBuilder.h"
#include "NoteSpec.h"

//////////////////////// 
// Base Pattern Layer //
////////////////////////
///////////////////////////////////////////////
// Pattern syntax:                           //
// 'x' or 'X' : play note (X = accented)     //
// '.'         : hold previous note          //
// '-' or any other char : rest              //
///////////////////////////////////////////////
struct PatternLayer
{
    int bars = 1;              
    float step_beats = 0.25f;   
    std::string pattern;
    NoteSpec note;

    void Emit(SequenceBuilder& sequence_builder, const float start_beat = 0.0f) const
    {
        if (pattern.empty() || bars <= 0 || step_beats <= 0.0f) return;

        const MusicalContext& context = sequence_builder.GetContext();
        const float total_beats = static_cast<float>(bars) * static_cast<float>(context.beats_per_bar);
        const int total_steps = static_cast<int>(total_beats / step_beats + 0.5f);

        bool has_pending = false;
        NoteSpec pending;
        float pending_start = 0.0f;
        float pending_dur = 0.0f;

        auto flush = [&]()
        {
            if (!has_pending) return;

            // percussive sounds ignore musical duration
            if (pending.articulation == Articulation::Percussive)
            {
                pending.duration_beats = step_beats;
            }
            else
            {
                pending.duration_beats = pending_dur;
            }

            sequence_builder.Add(pending_start, pending);
            has_pending = false;
            pending_dur = 0.0f;
        };


        for (int step_index = 0; step_index < total_steps; ++step_index)
        {
            const char step_char = pattern[step_index % pattern.size()];
            const float step_beat = start_beat + static_cast<float>(step_index) * step_beats;

            // start note
            if (step_char == 'x' || step_char == 'X')
            {
                flush();

                pending = note;
                pending_start = step_beat;
                pending_dur = step_beats;

                if (step_char == 'X') pending.velocity *= 1.8f;

                has_pending = true;
            }

            // hold
            else if (step_char == '.')
            {
                if (has_pending)
                {
                    // percussive sounds do not sustain
                    if (pending.articulation == Articulation::Sustained)
                    {
                        pending_dur += step_beats;
                    }
                    else
                    {
                        // treat hold as rest for percussive voices
                        flush();
                    }
                }
            }

            //  rest
            else
            {
                flush();
            }
        }
        flush();
    }

    void EmitAtBar(SequenceBuilder& sequence_builder, const int start_bar) const
    {
        const float start_beat = static_cast<float>(start_bar) * static_cast<float>(sequence_builder.GetContext().beats_per_bar);
        Emit(sequence_builder, start_beat);
    }
};
