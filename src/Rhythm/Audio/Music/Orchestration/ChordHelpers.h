#pragma once
#include <vector>

#include "NoteSpec.h"
#include "Audio/Music/Events/Midi.h"

///////////////////
// CHORD HELPERS //
///////////////////
// How to use:
// 1) Create a base NoteSpec with desired properties (duration, velocity, etc)
// 2) Call one of the Chords:: functions with the base NoteSpec, root pitch and octave
// 3) The function will return a vector of NoteSpecs representing the chord

namespace Chords
{
    // internal utilities
    inline NoteSpec MakeNote(const NoteSpec& base, const Pitch pitch, const int octave)
    {
        NoteSpec note = base;
        note.pitch = pitch;
        note.octave = octave;
        return note;
    }

    // step pitch chromatically using MIDI, return pitch + octave
    inline void StepPitchBySemitones(const Pitch base_pitch, const int base_octave, const int semitones, Pitch& out_pitch, int& out_octave)
    {
        int midi = PitchToMidi(base_pitch, base_octave);
        midi += semitones;

        out_octave = midi / 12 - 1;
        out_pitch = MidiToPitchClass(midi);
    }

    ////////////
    // Triads //
    ////////////
    inline std::vector<NoteSpec> Major(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5;
        int o3, o5;

        StepPitchBySemitones(root, octave, 4, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);

        // 1, 3, 5 
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5)
        };
    }

    inline std::vector<NoteSpec> Minor(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5;
        int o3, o5;

        StepPitchBySemitones(root, octave, 3, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);

        // 1, b3, 5
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5)
        };
    }

    inline std::vector<NoteSpec> Diminished(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5;
        int o3, o5;

        StepPitchBySemitones(root, octave, 3, p3, o3);
        StepPitchBySemitones(root, octave, 6, p5, o5);

        // 1, b3, b5
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5)
        };
    }

    inline std::vector<NoteSpec> Augmented(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5;
        int o3, o5;

        StepPitchBySemitones(root, octave, 4, p3, o3);
        StepPitchBySemitones(root, octave, 8, p5, o5);

        // 1, 3, #5
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5)
        };
    }

    inline std::vector<NoteSpec> Sus2(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p2, p5;
        int o2, o5;

        StepPitchBySemitones(root, octave, 2, p2, o2);
        StepPitchBySemitones(root, octave, 7, p5, o5);

        // 1, 2, 5
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p2, o2),
            MakeNote(base, p5, o5)
        };
    }

    inline std::vector<NoteSpec> Sus4(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p4, p5;
        int o4, o5;

        StepPitchBySemitones(root, octave, 5, p4, o4);
        StepPitchBySemitones(root, octave, 7, p5, o5);

        // 1, 4, 5
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p4, o4),
            MakeNote(base, p5, o5)
        };
    }

    //////////////
    // Sevenths //
    //////////////
    inline std::vector<NoteSpec> Dominant7(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5, p7;
        int o3, o5, o7;

        StepPitchBySemitones(root, octave, 4, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);
        StepPitchBySemitones(root, octave, 10, p7, o7);

        // 1, 3, 5, b7
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5),
            MakeNote(base, p7, o7)
        };
    }

    inline std::vector<NoteSpec> Minor7(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5, p7;
        int o3, o5, o7;

        StepPitchBySemitones(root, octave, 3, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);
        StepPitchBySemitones(root, octave, 10, p7, o7);

        // 1, b3, 5, b7
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5),
            MakeNote(base, p7, o7)
        };
    }

    inline std::vector<NoteSpec> Major7(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5, p7;
        int o3, o5, o7;

        StepPitchBySemitones(root, octave, 4, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);
        StepPitchBySemitones(root, octave, 11, p7, o7);

        // 1, 3, 5, 7
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5),
            MakeNote(base, p7, o7)
        };
    }

    inline std::vector<NoteSpec> HalfDiminished7(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5, p7;
        int o3, o5, o7;

        StepPitchBySemitones(root, octave, 3, p3, o3);
        StepPitchBySemitones(root, octave, 6, p5, o5);
        StepPitchBySemitones(root, octave, 10, p7, o7);

        // 1, b3, b5, b7
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5),
            MakeNote(base, p7, o7)
        };
    }

    ///////////////////
    // Colour Chords //
    ///////////////////
    inline std::vector<NoteSpec> Power(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p5;
        int o5;

        StepPitchBySemitones(root, octave, 7, p5, o5);

        // 1, 5
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p5, o5)
        };
    }
    
    inline std::vector<NoteSpec> Add9(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5, p9;
        int o3, o5, o9;

        StepPitchBySemitones(root, octave, 4, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);
        StepPitchBySemitones(root, octave, 14, p9, o9);

        // 1, 3, 5, 9
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5),
            MakeNote(base, p9, o9)
        };
    }

    inline std::vector<NoteSpec> MinorAdd9(const NoteSpec& base, const Pitch root, const int octave)
    {
        Pitch p3, p5, p9;
        int o3, o5, o9;

        StepPitchBySemitones(root, octave, 3, p3, o3);
        StepPitchBySemitones(root, octave, 7, p5, o5);
        StepPitchBySemitones(root, octave, 14, p9, o9);

        // 1, b3, 5, 9
        return
    {
            MakeNote(base, root, octave),
            MakeNote(base, p3, o3),
            MakeNote(base, p5, o5),
            MakeNote(base, p9, o9)
        };
    }
}
