#pragma once
#include "MusicalContext.h"
#include "SequenceBuilder.h"
#include "Audio/Music/Orchestration/ChordHelpers.h"
#include "Audio/Music/Orchestration/ChordOrchestrator.h"
#include "Audio/Music/Render/MixSettings.h"
#include "Audio/Music/Orchestration/SectionBuilder.h"

////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// IN-GAME SONGS ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// This file contains the music being used in the game.                           //
// Each function builds an EventSequence representing a full song.                //
// The MixSettings parameter allows adjusting volume levels for different voices. //
////////////////////////////////////////////////////////////////////////////////////
// While these are the final songs used in-game, they also serve as test cases    //
// for the music composition system as a whole.                                   //
// The sequences here cover a variety of ways of using the SectionBuilder,        //
// MelodyBuilder, ChordPatterns, and drum patterns.                               //
// It is my hope that this system can be reused and extended for future projects. //
////////////////////////////////////////////////////////////////////////////////////

/////////////////////
// EASY LEVEL SONG //
/////////////////////
inline EventSequence MakeSong_Easy(const MixSettings& mix)
{
    MusicalContext context;
    context.bpm = 120.0f;
    context.beats_per_bar = 4;

    SequenceBuilder sequence_builder(context);
    SectionBuilder section_builder(sequence_builder);

    struct TestCase
    {
        const char* name;
        NoteDuration duration;
    };

    const TestCase tests[] =
        {
            {"Whole", NoteDuration::Whole},
            {"Half", NoteDuration::Half},
            {"Quarter", NoteDuration::Quarter},
            {"Eighth", NoteDuration::Eighth},
            {"Sixteenth", NoteDuration::Sixteenth},
            // { "ThirtySecond", NoteDuration::ThirtySecond },
        };

    section_builder.BeginSection("Count-In", 1).Drums("","","x---x---x---x---").End();
    for (const auto& test : tests)
    {
        section_builder.BeginSection(test.name, 4)
            .Drums(
                "x---x---x-x-x-xx----x--x-x-x-x----x",
                "----x-------x---",
                "xxx-x-xx-xx-x-x----xxx-xx--x--x-x--xxx--x-x---xxxxx"
            )
            .Harmony({
                {Chords::Major, Pitch::F, 3},
                {Chords::Major, Pitch::G, 3},
                {Chords::Minor, Pitch::E, 3},
                {Chords::Minor, Pitch::A, 3},
            })
            .Bass(
                "C2 . C2 . C2 . C2 .",
                test.duration
            )
            .Melody(
                // C-Major scale/arpeggios to verify note timings
                "C4 D4 E4 F4 G4 A4 B4 C5 C4 E4 G4 C5 G4 E4",
                test.duration,
                true
            )
            .End();
    }

    return sequence_builder.Build();
}

///////////////////////
// MEDIUM LEVEL SONG //
///////////////////////
inline EventSequence MakeSong_Medium(const MixSettings& mix)
{
    MusicalContext context;
    context.bpm = 132.0f;
    context.beats_per_bar = 4;

    SequenceBuilder sequence_builder(context);
    SectionBuilder section_builder(sequence_builder);

    auto ChordBase = []
        {
            NoteSpec n;
            n.voice = VoiceType::Chord;
            return n;
        };

    section_builder.BeginSection("Count-In", 1).Drums("","","x---x---x---x---").End();

    // Phase 1 - Intro
    section_builder.BeginSection("Intro", 4)
        .Drums("x-------x-------", "----------------", "----------------")
        .Harmony({
            {Chords::Augmented, Pitch::C, 3},
            {Chords::Diminished,Pitch::Cs,3},
            {Chords::Augmented, Pitch::D, 3},
            {Chords::Diminished,Pitch::Ds,3},
        })
        .Bass("C2 Cs2 D2 Ds2", NoteDuration::Whole)
        .End();

    // PHASE 2 - 8 bars
    section_builder.BeginSection("Groove", 8)
        .Drums("x---x--x-x---x--", "----x-------x---", "x-x-x-x-x-x-x-x-")
        .Harmony({
            {Chords::Minor, Pitch::E, 3},
            {Chords::MinorAdd9, Pitch::G, 3},
            {Chords::Major7, Pitch::C, 4},
            {Chords::Minor7, Pitch::A, 3},
            {Chords::Sus2, Pitch::Fs, 3},
            {Chords::Dominant7, Pitch::B, 3},
            {Chords::Minor, Pitch::E, 3},
            {Chords::Minor, Pitch::E, 3},
        })
        .Bass("E2 G2 C3 A2 Fs2 B2 E2 E2", NoteDuration::Whole)
        .End();

    // PHASE 3 - 8 bars
    section_builder.BeginSection("Build", 8)
        .Drums("x-------x-------", "--------x-------", "x-x-x-x-x-x-x-x-")
        .Harmony({
            {Chords::Minor7, Pitch::A, 3},
            {Chords::Major7, Pitch::C, 4},
            {Chords::Sus4,   Pitch::B, 3},
            {Chords::Minor,  Pitch::E, 3},
            {Chords::Minor7, Pitch::A, 3},
            {Chords::Major7, Pitch::C, 4},
            {Chords::Sus4,   Pitch::B, 3},
            {Chords::MinorAdd9, Pitch::E, 3},
        })
        .Bass("A2 C3 B2 E2 A2 C3 B2 E2", NoteDuration::Whole)
        .Melody(
            "E5 . D5 C5 | B4 ~ C5 D5 . | "
            "E5 . . . | B4 . . .",
            NoteDuration::Quarter,
            true
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Rhythm(sequence_builder, 12, 8, Chords::Power(base, Pitch::E, 3), 0.5f);
    }

    // PHASE 4 — 16 bars
    section_builder.BeginSection("Chaos", 16)
        .Drums("x-x-xx-x-x-xx-x-", "----x-------x-x-", "xxxxxxxxxxxxxxxx")
        .Harmony({
            {Chords::Minor, Pitch::E, 3},
            {Chords::Dominant7, Pitch::B, 3},
            {Chords::Minor7, Pitch::A, 3},
            {Chords::Major7, Pitch::C, 4},

            {Chords::Sus2, Pitch::Fs, 3},
            {Chords::Minor, Pitch::E, 3},
            {Chords::Dominant7, Pitch::B, 3},
            {Chords::MinorAdd9, Pitch::E, 3},

            {Chords::Minor, Pitch::E, 3},
            {Chords::Dominant7, Pitch::B, 3},
            {Chords::Minor7, Pitch::A, 3},
            {Chords::Major7, Pitch::C, 4},

            {Chords::Sus2, Pitch::Fs, 3},
            {Chords::Minor, Pitch::E, 3},
            {Chords::Dominant7, Pitch::B, 3},
            {Chords::MinorAdd9, Pitch::E, 3},
        })
        .Bass(
            "E2 B2 A2 C3 Fs2 E2 B2 E2 E2 B2 A2 C3 Fs2 E2 B2 E2",
            NoteDuration::Whole
        )
        .Melody(
            "E5 G5 A5 B5 | C6 ~ B5 A5 . | "
            "Fs5 . E5 . | B5 . . .",
            NoteDuration::Quarter,
            true
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Stab(sequence_builder, 20, 16, Chords::Power(base, Pitch::E, 3), 0.25f);
        ChordOrchestrator::Stab(sequence_builder, 20, 16, Chords::Power(base, Pitch::B, 3), 0.25f);
    }

    // PHASE 5 — 8 bars
    section_builder.BeginSection("Breathe", 8)
        .Drums("x---x---x---x---", "----x-------x---", "x-x-x-x-x-x-x-x-")
        .Harmony({
            {Chords::Major, Pitch::C, 4},
            {Chords::Major, Pitch::G, 3},
            {Chords::Minor, Pitch::A, 3},
            {Chords::Major, Pitch::F, 3},
            {Chords::Major7,Pitch::C, 4},
            {Chords::Dominant7,Pitch::G, 3},
            {Chords::Major, Pitch::C, 4},
            {Chords::Major, Pitch::C, 4},
        })
        .Bass("C3 G2 A2 F2 C3 G2 C3 C3", NoteDuration::Whole)
        .End();

    return sequence_builder.Build();
}

///////////////////////
// BRUTAL LEVEL SONG //
///////////////////////
inline EventSequence MakeSong_Brutal(const MixSettings& mix)
{
    MusicalContext context;
    context.bpm = 138.0f;
    context.beats_per_bar = 4;

    SequenceBuilder sequence_builder(context);
    SectionBuilder section_builder(sequence_builder);

    auto ChordBase = []
    {
            NoteSpec n;
            n.voice = VoiceType::Chord;
            return n;
    };

    // Count-in
    section_builder.BeginSection("CountIn", 1)
        .Drums("","", "x-x-x-x-x-x-x-x-")
        .End();

    section_builder.BeginSection("Intro1", 4)
        .Drums("x---x---x---x---", "----x-------x---", "----------------")
        .Bass("E0 E0 E0 E0", NoteDuration::Whole)
        .Melody("E2 - - - | E2 - - - | E2 - - - | E2 - - -", NoteDuration::Quarter, false)
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Stab(sequence_builder, 1, 4, Chords::Power(base, Pitch::E, 2), 0.75f);
    }
    
    // INTRO — 8 bars
    section_builder.BeginSection("Intro2", 8)
        .Drums(
            "x--x---x--x-x---",
            "----x-------x---",
            "----------------"
        )
        .Melody(
            "E2 F2 F#2 G2 B1 C2 D#2 E2 | "
            "E2 F2 F#2 G2 B1 C2 D#2 E2 | "
            "E2 F2 F#2 G2 B1 C2 D#2 E2 | "
            "E2 F2 F#2 G2 B1 C2 D#2 E2",
            NoteDuration::Eighth,
            true
        )
        .Bass(
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 F1 F#1 G1 B0 C1 D#1 E1",
            NoteDuration::Eighth
        )
        .End();
    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Stab(sequence_builder, 5, 8, Chords::Power(base, Pitch::E, 2), 0.20f);
        ChordOrchestrator::Stab(sequence_builder, 5, 8, Chords::Power(base, Pitch::B, 2), 0.12f);
    }

    // VERSE A — 16 bars
    section_builder.BeginSection("VerseA", 16)
        .Drums(
            "x-x--xx---x-x-xx",
            "----x-------x---",
            "----------------"
        )
        .Melody(
            "E2 F2 F#2 G2 B1 C2 D#2 E2 | "
            "E2 . D#2 E2 F2 F#2 . G2 | "
            "B1 C2 D#2 E2 B1 . C2 . | "
            "E2 F2 F#2 G2 B1 C2 D#2 E2",
            NoteDuration::Eighth,
            true
        )
        .Bass(
            "E1 F1 Fs1 G1 B0 C1 Ds1 E1 "
            "E1 . D#1 E1 F1 F#1 . G1 "
            "B0 C1 D#1 E1 B0 . C1 . "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "

            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 . D#1 E1 F1 F#1 . G1 "
            "B0 C1 D#1 E1 B0 . C1 . "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "

            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 . D#1 E1 F1 F#1 . G1 "
            "B0 C1 D#1 E1 B0 . C1 . "
            "E1 F1 F#1 G1 B0 C1 D#1 E1 "

            "E1 F1 F#1 G1 B0 C1 D#1 E1 "
            "E1 . D#1 E1 F1 F#1 . G1 "
            "B0 C1 D#1 E1 B0 . C1 . "
            "E1 F1 F#1 G1 B0 C1 D#1 E1",
            NoteDuration::Eighth
        )
        .End();
    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Rhythm(sequence_builder, 13, 16, Chords::Power(base, Pitch::E, 2), 1.0f);
        ChordOrchestrator::Rhythm(sequence_builder, 13, 16, Chords::Diminished(base, Pitch::Fs, 3), 2.0f);
    }

    // PRE-CHORUS — 8 bars
    section_builder.BeginSection("PreChorus", 8)
        .Drums(
            "x---x---x---x---",
            "--------x-------",
            "----------------"
        )
        .Melody(
            "E2 F2 F#2 G2 | B1 C2 D#1 E2 | "
            "E2 F2 Fs2 G2 | B1 C2 D#2 E2",
            NoteDuration::Sixteenth,
            true
        )
        .Bass(
            "E1 F1 F#1 G1 B0 C1 D#1 E1",
            NoteDuration::Whole
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Stab(sequence_builder, 29, 8, Chords::HalfDiminished7(base, Pitch::Fs, 3), 0.25f);
        ChordOrchestrator::Stab(sequence_builder, 29, 8, Chords::Power(base, Pitch::B, 2), 0.18f);
    }

    // CHORUS — 16 bars
    section_builder.BeginSection("Chorus", 16)
        .Drums(
            "x-x-xx-x-x-x-xx-",
            "----x-------x-x-",
            "x-x-x-x-x-x-x-x-"
        )
        .Melody(
            "E4 G4 B4 C5 | B4 G4 E4 . | "
            "E4 G4 B4 D#5 | C5 B4 G4 . | "
            "E4 G4 B4 C5 | B4 G4 E4 G4 | "
            "A#4 G4 B4 D#5 | C5 B4 G4 F#3 |"
            "E5 G4 B4 C3 | B5 G2 E3 . | "
            "E4 G4 B4 D#3 | E5 A#4 F#4 . | "
            "E4 G4 B4 C5 | B4 G4 E4 G4 | "
            "A#4 G4 B4 D#5 | C5 A#4 G4 F#3 |",
            NoteDuration::Sixteenth,
            true
        )
        .Bass(
            "E1 E1 B0 C1 E1 E1 B0 D#1 "
            "E1 E1 B0 C1 E1 E1 B0 D#1",
            NoteDuration::Whole
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Rhythm(sequence_builder, 37, 16, Chords::Power(base, Pitch::E, 2), 0.5f);
        ChordOrchestrator::Stab(sequence_builder,   37, 16, Chords::Power(base, Pitch::B, 2), 0.20f);
    }

    // POST-CHORUS — 8 bars 
    section_builder.BeginSection("PostChorus", 8)
        .Drums(
            "x---x-x---x---x-",
            "----x-------x---",
            "----------------"
        )
        .Melody(
            "E1 . F1 F#1 | E1 . D#1 E1 | "
            "E1 . F1 F#1 | B0 C1 D#1 E1",
            NoteDuration::Quarter,
            false
        )
        .Bass(
            "E0 E0 F0 Fs0 E0 E0 D#0 E0",
            NoteDuration::Whole
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Stab(sequence_builder, 53, 8, Chords::Power(base, Pitch::E, 2), 0.35f);
    }

    // VERSE B — 16 bars
    section_builder.BeginSection("VerseB", 16)
        .Drums(
            "x--x-xx--x--x-x",
            "----x-------x---",
            "----------------"
        )
        .Melody(
            "F#2 G2 B1 C2 | D#2 E2 . . | "
            "E2 D#2 E2 F2 | F#2 G2 . . | "
            "B1 C2 D#2 E2 | B1 . C2 . | "
            "F#2 G2 B1 C2 | D#2 E2 . .",
            NoteDuration::Eighth,
            true
        )
        .Bass(
            "F#1 G1 B0 C1 D#1 E1 - - "
            "E1 D#1 E1 F1 F#1 G1 - - "
            "B0 C1 D#1 E1 B0 - C1 - "
            "F#1 G1 B0 C1 D#1 E1 - - "

            "F#1 G1 B0 C1 D#1 E1 - - "
            "E1 D#1 E1 F1 F#1 G1 - - "
            "B0 C1 D#1 E1 B0 - C1 - "
            "F#1 G1 B0 C1 D#1 E1 - - "

            "F#1 G1 B0 C1 D#1 E1 - - "
            "E1 D#1 E1 F1 F#1 G1 - - "
            "B0 C1 D#1 E1 B0 - C1 - "
            "F#1 G1 B0 C1 D#1 E1 - - "

            "F#1 G1 B0 C1 D#1 E1 - - "
            "E1 D#1 E1 F1 F#1 G1 - - "
            "B0 C1 D#1 E1 B0 - C1 - "
            "F#1 G1 B0 C1 D#1 E1 - -",
            NoteDuration::Eighth
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Rhythm(sequence_builder, 61, 16, Chords::Power(base, Pitch::Fs, 2), 1.0f);
        ChordOrchestrator::Stab(sequence_builder,   61, 16, Chords::Diminished(base, Pitch::G, 3), 0.22f);
    }

    // PRE-CHORUS — 8 bars
    section_builder.BeginSection("PreChorus2", 8)
        .Drums("x---x---x---x---", "--------x-------", "----------------")
        .Melody(
            "E2 F2 F#2 G2 | B1 C2 D#2 E2 | "
            "E2 F2 F#2 G2 | B1 C2 D#2 E2",
            NoteDuration::ThirtySecond,
            true
        )
        .Bass("E1 F1 F#1 G1 B0 C1 D#1 E1", NoteDuration::Whole)
        .End();

    // CHORUS — 16 bars
    section_builder.BeginSection("Chorus2", 16)
        .Drums(
            "x-x-xx-x-x-x-xx-",
            "----x-------x-x-",
            "x-x-x-x-x-x-x-x-"
        )
        .Melody(
            "E4 G4 B4 C5 | B4 G4 E4 . | "
            "E4 G4 B4 D#5 | C5 B4 G4 . | "
            "E4 G4 B4 C5 | B4 G4 E4 G4 | "
            "A#4 G4 B4 D#5 | C5 B4 G4 F#3 |"
            "E5 G4 B4 C3 | B5 G2 E3 . | "
            "E4 G4 B4 D#3 | E5 A#4 F#4 . | "
            "E4 G4 B4 C5 | B4 G4 E4 G4 | "
            "A#4 G4 B4 D#5 | C5 A#4 G4 F#3 |",
            NoteDuration::Sixteenth,
            true
        )
        .Bass(
            "E1 E1 B0 C1 E1 E1 B0 D#1 "
            "E1 E1 B0 C1 E1 E1 B0 D#1",
            NoteDuration::Whole
        )
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Rhythm(sequence_builder, 93, 16, Chords::Power(base, Pitch::E, 2), 0.5f);
    }

    // OUTRO A — 4 bars
    section_builder.BeginSection("OutroA", 4)
        .Drums("x---x---x---x---", "----x-------x---", "----------------")
        .Bass("E0 E0 E0 E0", NoteDuration::Whole)
        .Melody("E2 - - - | E2 - - - | E2 - - - | E2 - - -", NoteDuration::Quarter, false)
        .End();

    {
        NoteSpec base = ChordBase();
        ChordOrchestrator::Stab(sequence_builder, 109, 4, Chords::Power(base, Pitch::E, 2), 0.75f);
    }

    section_builder.BeginSection("OutroB", 4)
        .Drums("x--xx-----x-x-x-", "----x-------x---", "xxx-xxx-xxx-xxx-")
        .Bass("E0 E0 E0 E0", NoteDuration::Whole)
        .Melody("E2 - - - | E2 - - - | E2 - - - | E2 - - -", NoteDuration::Quarter, false)
        .End();
    return sequence_builder.Build();
}


/////////////////////
// HARD LEVEL SONG //
/////////////////////
inline EventSequence MakeSong_Hard(const MixSettings& mix)
{
    MusicalContext context;
    context.bpm = 138.0f;
    context.beats_per_bar = 4;

    SequenceBuilder sequence_builder(context);
    SectionBuilder section_builder(sequence_builder);
    const std::string chug_a = "x-x-x-x-x-x-x-x-";
    const std::string chug_b = "x--x-x--x--x-x--";
    const std::string chug_c = "x---x-x---x-x---"; 
    const std::string hats_8 = "x-x-x-x-x-x-x-x-";

    // COUNT-IN — 1 bar
    section_builder.BeginSection("CountIn", 1)
        .Drums("", "", hats_8)
        .End();

    // INTRO — 8 bars 
    section_builder.BeginSection("Intro", 8)
        .Drums(chug_c, "----x-------x---", hats_8)
        .Bass(
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . Fs1 . G1 . Fs1 . | "
            "E1 . Ds1 . E1 . F1  . | "
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . Fs1 . G1 . Fs1 . | "
            "E1 . Ds1 . E1 . .  .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_c, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.85f)
        .ChordRhythm("----x-------x---", NoteDuration::Sixteenth, Chords::Power, Pitch::F, 2, 0.70f)
        .End();

    // VERSE 1 — 8 bars 
    section_builder.BeginSection("Verse1", 8)
        .Drums(chug_b, "----x-------x---", hats_8)
        .Bass(
            "E1 . B0 . E1 . Ds1 . | "
            "E1 . B0 . F1 . E1  . | "
            "E1 . C1 . E1 . Ds1 . | "
            "E1 . B0 . F1 . .   . | "
            "E1 . B0 . E1 . Ds1 . | "
            "E1 . B0 . F1 . E1  . | "
            "E1 . C1 . E1 . Ds1 . | "
            "E1 . B0 . F1 . .   .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_b, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.90f)
        .End();

    // PRE-CHORUS — 4 bars 
    section_builder.BeginSection("PreChorus", 4)
        .Drums("x---x---x---x---", "--------x-------", "xx-xx-xx-xx-xx-x")
        .Bass("C1 . . . | Ds1 . . . | Fs1 . . . | F1 . . .", NoteDuration::Quarter)
        .Harmony({
            {Chords::Minor7,          Pitch::C,  3},
            {Chords::HalfDiminished7, Pitch::Ds, 3},
            {Chords::Minor7,          Pitch::Fs, 3},
            {Chords::Dominant7,       Pitch::F,  3},
        })
        .End();

    // CHORUS — 8 bars
    section_builder.BeginSection("Chorus", 8)
        .Drums(chug_a, "----x-------x---", "xxx-xxx-x-xx-xx-")
        .Bass(
            "E1 . E1 . B0 . B0 . | "
            "C1 . C1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "Ds1 . Ds1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "C1 . C1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "Ds1 . Ds1 . B0 . .  .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_a, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.95f)
        .Melody(
            "E4 - - B3  E4 - - -  G4 - - B3  - - - - | "
            "E4 - - -   Ds4 - - B3  - - - -   E4 - - - | "
            "E4 - - B3  E4 - - -  As4 - - G4  - - - - | "
            "Fs4 - - E4  - - Ds4 -  - - - -   B3 - - - | "
            "E4 - - B3  E4 - - -  G4 - - B3  - - - - | "
            "E4 - - -   C4  - - As3 - - - -  E4 - - - | "
            "E4 - - B3  E4 - - -  As4 - - G4  - - - - | "
            "B3 - - -   E4 - - -  - - - -   E4 . . .",
            NoteDuration::Sixteenth,
            false
        )
        .End();

    // WEIRD BREAK — 8 bars
    section_builder.BeginSection("WeirdBreak", 8)
        .Drums("x--x-xx---x-xx--", "----x-------x---", "x-xxx-x---x-xxx-")
        .Bass(
            "E1 . . . | F1 . . . | Fs1 . . . | G1 . . . | "
            "E1 . . . | Ds1 . . . | C1 . . . | B0 . . .",
            NoteDuration::Quarter
        )
        .ChordRhythm("x--x-xx---x-xx--", NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.98f)
        .ChordRhythm("----x-------x---", NoteDuration::Sixteenth, Chords::HalfDiminished7, Pitch::Ds, 3, 0.55f)
        .End();

    // VERSE 2 — 8 bars
    section_builder.BeginSection("Verse2", 8)
        .Drums(chug_b, "----x-------x---", hats_8)
        .Bass(
            "E1 . C1 . E1 . Ds1 . | "
            "Fs1 . E1 . C1 . Ds1 . | "
            "G1 . Ds1 . Fs1 . E1  . | "
            "F1 . E1 . C1 . B0  . | "
            "E1 . C1 . E1 . Ds1 . | "
            "Fs1 . E1 . C1 . Ds1 . | "
            "G1 . Ds1 . Fs1 . E1  . | "
            "F1 . E1 . C1 . .   .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_b, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.90f)
        .Harmony({
            {Chords::Minor7,          Pitch::E,  3}, 
            {Chords::Major7,          Pitch::C,  3}, 
            {Chords::HalfDiminished7, Pitch::Fs, 3}, 
            {Chords::Dominant7,       Pitch::F,  3}, 
            {Chords::Minor7,          Pitch::E,  3},
            {Chords::Minor7,          Pitch::Ds, 3}, 
            {Chords::Minor7,          Pitch::G,  3}, 
            {Chords::Dominant7,       Pitch::C,  3}, 
        })
        .ChordRhythm("----x-x-----x-x-", NoteDuration::Sixteenth, Chords::MinorAdd9, Pitch::C, 4, 0.42f)
        .End();

    // PRE-CHORUS VAR — 4 bars 
    section_builder.BeginSection("PreChorusVar", 4)
        .Drums("x---x--x---x--x-", "--------x-------", "x---xxx-x---x-xx")
        .Bass("C1 . . . | Ds1 . . . | G1 . . . | Fs1 . . .", NoteDuration::Quarter)
        .Harmony({
            {Chords::Minor7,    Pitch::C,  3},
            {Chords::Dominant7, Pitch::Ds, 3},
            {Chords::Minor7,    Pitch::G,  3},
            {Chords::Major7,    Pitch::Fs, 3},
        })
        .Melody("B3 - E4 -", NoteDuration::Eighth, true)
        .End();

    // CHORUS 2 — 8 bars
    section_builder.BeginSection("Chorus2", 8)
        .Drums(chug_a, "----x-------x---", hats_8)
        .Bass(
            "E1 . E1 . B0 . B0 . | "
            "C1 . C1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "Ds1 . Ds1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "C1 . C1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "Ds1 . Ds1 . B0 . .  .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_a, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.95f)
        .Melody(
            "E4 - - B3  E4 - - -  G4 - - B3  - - - - | "
            "E4 - - -   Ds4 - - B3  - - - -   E4 - - - | "
            "E4 - - B3  E4 - - -  As4 - - G4  - - - - | "
            "Fs4 - - E4  - - Ds4 -  - - - -   B3 - - - | "
            "E4 - - B3  E4 - - -  G4 - - B3  - - - - | "
            "E4 - - -   C4  - - As3 - - - -  E4 - - - | "
            "E4 - - B3  E4 - - -  As4 - - G4  - - - - | "
            "B3 - - -   E4 - - -  - - - -   E4 . . .",
            NoteDuration::Sixteenth,
            false
        )
        .End();

    // HEAVY BREAKDOWN — 16 bars
    section_builder.BeginSection("HeavyBreakA", 8)
        .Drums("x-xx-xx-x-xx-xx-x", "----x-------x---", "----------------")
        .Bass(
            "E1 . . . | E1 . . . | F1 . . . | E1 . . . | "
            "Ds1 . . . | E1 . . . | C1 . . . | B0 . . .",
            NoteDuration::Quarter
        )
        .ChordRhythm("x-xx-xx-x-xx-xx-x", NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 1.00f)
        .End();

    section_builder.BeginSection("HeavyBreakB", 8)
        .Drums("x-xx-xx-x-xx-xx-x", "----x-------x---", hats_8)
        .Bass(
            "E1 . . . | E1 . . . | F1 . . . | E1 . . . | "
            "Ds1 . . . | E1 . . . | C1 . . . | B0 . . .",
            NoteDuration::Quarter
        )
        .ChordRhythm("x-xx-xx-x-xx-xx-x", NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 1.00f)
        .Melody(
            "E4 - - B3  E4 - - -  G4 - - -  B3 - - - | "
            "E4 - - -   Ds4 - - B3  - - - -   E4 - - - | "
            "E4 - - B3  E4 - - -  As4 - - -  G4 - - - | "
            "Fs4 - - E4  - - Ds4 -  - - - -   B3 - - - | "
            "E4 - - B3  E4 - - -  G4 - - -  B3 - - - | "
            "E4 - - -   C4  - - As3 - - - -  E4 - - - | "
            "E4 - - B3  E4 - - -  As4 - - -  G4 - - - | "
            "B3 - - -   E4 - - -  - - - -   E4 . . .",
            NoteDuration::Sixteenth,
            false
        )
        .End();

    // CHORUS VAR — 16 bars 
    section_builder.BeginSection("ChorusVarA", 8)
        .Drums("x---x--x---x--x-", "----x-------x-x-", hats_8)
        .Bass(
            "E1 . B0 . E1 . Ds1 . | "
            "C1 . B0 . C1 . B0  . | "
            "E1 . B0 . E1 . Ds1 . | "
            "Ds1 . B0 . Ds1 . B0 . | "
            "E1 . B0 . E1 . Ds1 . | "
            "C1 . B0 . C1 . B0  . | "
            "E1 . B0 . E1 . Ds1 . | "
            "Ds1 . B0 . E1 . .  .",
            NoteDuration::Eighth
        )
        .End();

    section_builder.BeginSection("ChorusVarB", 8)
        .Drums(chug_a, "----x-------x-x-", "x-x-xxx-x-xx-xx-")
        .Bass(
            "E1 . E1 . B0 . B0 . | "
            "C1 . C1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "Ds1 . Ds1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "C1 . C1 . B0 . B0 . | "
            "E1 . E1 . B0 . B0 . | "
            "Ds1 . Ds1 . B0 . .  .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_a, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.98f)
        .Melody(
            "E4 - - -   B3 - E4 -  G4 - - -   B3 - - - | "
            "E4 - Ds4 -  - - B3 -  - - - -    E4 - - - | "
            "E4 - - -   B3 - E4 -  As4 - - -  G4 - - - | "
            "Fs4 - - E4  - - Ds4 - - - - -    B3 - - - | "
            "E4 - - -   B3 - E4 -  G4 - - -   B3 - - - | "
            "E4 - C4  -  - - As3 - - - - -    E4 - - - | "
            "E4 - - -   B3 - E4 -  As4 - - -  G4 - - - | "
            "B3 - - -   E4 - - -   - - - -    E4 . . .",
            NoteDuration::Sixteenth,
            false
        )
        .End();

    // OUTRO — 8 bars
    section_builder.BeginSection("Outro", 8)
        .Drums(chug_c, "----x-------x---", hats_8)
        .Bass(
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . Fs1 . G1 . Fs1 . | "
            "E1 . Ds1 . E1 . F1  . | "
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . F1 . E1 . Ds1 . | "
            "E1 . Fs1 . G1 . Fs1 . | "
            "E1 . Ds1 . E1 . .  .",
            NoteDuration::Eighth
        )
        .ChordRhythm(chug_c, NoteDuration::Sixteenth, Chords::Power, Pitch::E, 2, 0.80f)
        .End();

    return sequence_builder.Build();
}

