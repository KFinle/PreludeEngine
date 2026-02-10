#pragma once
#include "Audio/Music/Orchestration/SectionBuilder.h"
#include "ChordHelpers.h"
#include "MusicalContext.h"
#include "SequenceBuilder.h"
#include "Audio/Music/Render/MixSettings.h"

////////////////////
// Test Sequences //
/////////////////////////////////////////////////////////////
// Used to test the various features of the CompositionAPI //
/////////////////////////////////////////////////////////////


// test sequence: note pool stress test
inline EventSequence MakeSong_NotePoolTest(MixSettings& mix)
{
    MusicalContext context;
    context.bpm = 300;
    context.beats_per_bar = 4;

    SequenceBuilder sequence_builder(context);
    SectionBuilder section_builder(sequence_builder);

    section_builder.BeginSection("Count-In", 2).Drums("","","x---x---x---x---").End();
    
    section_builder.BeginSection("NotePoolTest", 30)
    .Kick("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")
    .Snare("----x-------x-------x-------x---")
    .Hat("x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-x-")
                .Melody(
                // C-Major scale/arpeggio to verify note timings
                "C4 D4 E4 F4 G4 A4 B4 C5 C4 E4 G4 C5 G4 E4",
                NoteDuration::ThirtySecond,
                true
            )
            .Bass(
                // Root notes to verify note pool handling
                "C2 - - - F2 - - - G2 - - - C3 - - - ",
                NoteDuration::Sixteenth,
                true
            )
            .ChordRhythm(
                // Full chords to verify chord note pooling
                "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
                NoteDuration::Eighth,
                Chords::Major,
                Pitch::C,
                3,
                0.2f
            )
    
     .End();

    return sequence_builder.Build(mix);
}

// test sequence: composition API tests
inline EventSequence MakeSong_CompositionAPITests(const MixSettings& mix)
{
    MusicalContext context;
    context.bpm = 120;
    context.beats_per_bar = 4;

    SequenceBuilder sequence_builder(context);
    SectionBuilder section_builder(sequence_builder);

    // 1. TIME
    section_builder.BeginSection("01_Timebase", 2)
     .Melody(
        "C4 . D4 . E4 . F4 .",
        NoteDuration::Quarter,
        false
     )
     .Bass(
        "- C2 - D2 - E2 - F2 ",
        NoteDuration::Eighth,
        true
     )
     .End();

    // 2. PATTERN LOOPING
    section_builder.BeginSection("02_PatternLoop", 4)
     .Drums(
        "x---",
        "----",
        "x-x-"
     )
     .End();

    // 3. PATTERN HOLDS
    section_builder.BeginSection("03_PatternHolds", 2)
     .ChordRhythm(
        "X..-",
        NoteDuration::Quarter,
        Chords::Power,
        Pitch::E,
        2,
        0.8f
     )
     .End();

    // 4. PATTERN ACCENTS
    section_builder.BeginSection("04_PatternAccents", 1)
     .Drums(
        "x_xx_x",
        "----",
        "----"
     )
     .End();

    // 5. CHORD RHYTHM + HOLDS
    section_builder.BeginSection("05_ChordRhythm", 2)
     .ChordRhythm(
        "X.x_x..x-",
        NoteDuration::Eighth,
        Chords::Power,
        Pitch::Fs,
        2,
        0.6f
     )
     .End();

    // 6. MELODY SLIDES
    section_builder.BeginSection("06_MelodySlides", 2)
     .Melody(
        "E4 ~ G4 . A4 ~ C5 .",
        NoteDuration::Quarter,
        false
     )
     .End();

    // this doesn't work right now. Should fix, but that's an architectural refactor I don't have time for
    // 7. 3/4 TIME SIGNATURE TEST // 
    // context.beats_per_bar = 3;

    section_builder.BeginSection("07_ThreeFour", 2)
     .Drums(
        "x--x--x--",
        "---x-----",
        "x-x-x-x-x"
     )
     .Melody(
        "E3 F3 G3 A3 B3 C4",
        NoteDuration::Eighth,
        false
     )
     .End();

    // context.beats_per_bar = 4;

    // 8. MIXED GRIDS (16ths + quarters)
    section_builder.BeginSection("08_MixedGrids", 2)
     .Drums(
        "x---x---x---x---",
        "----x-------x---",
        "x-x-x-x-x-x-x-x-"
     )
     .Melody(
        "E4 . G4 . B4 . C5 .",
        NoteDuration::Quarter,
        false
     )
     .End();

    // 9. SECTION SCOPING
    section_builder.BeginSection("09_ScopeA", 1)
     .Drums("x---", "----", "----")
     .End();

    section_builder.BeginSection("09_ScopeB", 1)
     .Drums("----", "x---", "----")
     .End();

    section_builder.BeginSection("09_ScopeC", 1)
     .Drums("----", "----", "x---")
     .End();

    // 10. FULL INTEGRATION STRESS TEST
    section_builder.BeginSection("10_FullIntegration", 4)
     .Drums(
        "x--x-x--xx--x-x-",
        "----x-------x---",
        "x-x-x-x-x-x-x-x-"
     )
     .Bass(
        "E2 . F2 Fs2 G2 . Fs2 E2",
        NoteDuration::Eighth,
        true
     )
     .ChordRhythm(
        "X.xx.---Xx-",
        NoteDuration::Sixteenth,
        Chords::Power,
        Pitch::E,
        2,
        0.7f
     )
     .Melody(
        "E4 ~ G4 . A4 . G4 .",
        NoteDuration::Quarter,
        true
     )
     .End();

    return sequence_builder.Build();
}
