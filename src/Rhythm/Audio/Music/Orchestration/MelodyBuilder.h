#pragma once
#include <string>
#include <vector>
#include "SequenceBuilder.h"
#include "NoteSpec.h"

///////////////////
// MelodyBuilder //
///////////////////
// Grammar 
// - Note token: [Pitch][Optional Accidental][Octave] e.green. C4, D#5
// - Rest token: "-" (silence 1 step)
// - Hold token: "." (extend previous note by 1 step)
// - Slide token: "~" (bend pitch from one note to another) 
//
// Step grid:
// - step_beats defines how many beats each token advances (except hold extends).
namespace MelodyBuilder
{
    struct Settings
    {
        float start_beat = 0.0f;
        int bars = 1;
        NoteDuration step_duration = NoteDuration::Sixteenth;
        float step_beats = 0.25f;
        bool loop_tokens = true;
        bool strict = false;

        // how should the end of a phrase be handled?
        enum class EndMode { Stop, RestFill, HoldFill };
        EndMode end_mode = EndMode::RestFill;
    };


    // Emit a monophonic melody line using a base NoteSpec
    // Pitch & octave come from the pattern string tokens.
    //
    // Example:
    //   NoteSpec lead = Nes::PulseLead();
    //   Emit(b, lead, "C4 . . -  E4 . - -  G4 . . .", { .step_beats = NoteDuration::Quarter });
    //
    void EmitMonophonic(SequenceBuilder& b, const NoteSpec& base, const std::string& pattern, const Settings& s = {});

    // Emit a chord at a given beat.
    void EmitChord(SequenceBuilder& b, float start_beat, const std::vector<NoteSpec>& notes, float duration_beats);
}
