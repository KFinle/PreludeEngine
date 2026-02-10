#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include "SequenceBuilder.h"
#include "MelodyBuilder.h"
#include "ChordSequence.h"
#include "ChordPattern.h"
#include "Audio/Music/Instruments/NesInstruments.h"
#include "Audio/Music/Orchestration/DrumPattern.h"

/////////////////////////////////////
// SectionBuilder                  //
// High-level musical form builder //
///////////////////////////////////////////////////////////////////////////////////////////
// Usage:                                                                                //
//   SequenceBuilder seq(ctx);                                                           //
//   SectionBuilder section(seq);                                                        //
//   section.BeginSection("Intro", 4)                                                    //
//          .Harmony({ { Chords::Major, Pitch::C, 4 }, { Chords::Minor, Pitch::A, 3 } }) //
//          .Bass("C2 - E2 - G2 - - - ", NoteDuration::Quarter)                          //
//          .Melody("E4 F4 G4 - G4 F4 E4 - ", NoteDuration::Sixteenth)                   //
//          .Drums("x---x---x---x---", "----x-----------", "x-x-x-x-x-x-x-x-")           //
//          .End();                                                                      //
//   seq.Build();                                                                        //
///////////////////////////////////////////////////////////////////////////////////////////
class SectionBuilder
{
public:
    explicit SectionBuilder(SequenceBuilder& seq) : m_sequence(seq) {}
    using chord_fn = std::function<std::vector<NoteSpec>(const NoteSpec&, Pitch, int)>;
    

    /////////////////////
    // SECTION CONTROL //
    /////////////////////
    SectionBuilder& BeginSection(const std::string& name, const int bars)
    {
        m_section_name = name;
        m_section_bars = bars;
        m_section_start = m_bar_cursor;
        return *this;
    }

    void End()
    {
        m_bar_cursor += m_section_bars;
        m_section_bars = 0;
        m_section_name.clear();
    }

    /////////////
    // HARMONY //
    /////////////
    SectionBuilder& Harmony(const std::vector<std::tuple<chord_fn, Pitch, int>>& progression)
    {
        NoteSpec chord_base;
        chord_base.voice = VoiceType::Chord;

        int bar = 0;
        for (const auto& step : progression)
        {
            if (bar >= m_section_bars) break;

            const auto& chord_fn = std::get<0>(step);
            
            Pitch root = std::get<1>(step);
            int octave = std::get<2>(step);

            ChordSequence::EmitBars(m_sequence, m_section_start + bar, 1, chord_fn(chord_base, root, octave));
            bar++;
        }
        return *this;
    }

    //////////
    // BASS //
    //////////
    SectionBuilder& Bass(const std::string& pattern, const NoteDuration duration = NoteDuration::Quarter, const bool loop = false)
    {
        NoteSpec base = Nes::TriangleBass();

        MelodyBuilder::Settings settings;
        settings.start_beat = GetSectionStartBeat();
        settings.bars = m_section_bars;
        settings.step_duration = duration;
        settings.loop_tokens = loop;
        settings.end_mode = MelodyBuilder::Settings::EndMode::RestFill;

        MelodyBuilder::EmitMonophonic(m_sequence, base, pattern, settings);
        return *this;
    }


    ////////////
    // MELODY //
    ////////////
    SectionBuilder& Melody(const std::string& pattern, const NoteDuration duration = NoteDuration::Sixteenth, const bool loop = false)
    {
        NoteSpec base = Nes::PulseLead();

        MelodyBuilder::Settings settings;
        settings.start_beat = GetSectionStartBeat();
        settings.bars = m_section_bars;
        settings.step_duration = duration;
        settings.loop_tokens = loop;
        settings.end_mode = MelodyBuilder::Settings::EndMode::RestFill;

        MelodyBuilder::EmitMonophonic(m_sequence, base, pattern, settings);
        return *this;
    }


    ///////////
    // DRUMS //
    ///////////
    SectionBuilder& Drums(const std::string& kick, const std::string& snare, const std::string& hat)
    {
        KickPattern kick_pattern(kick, m_section_bars);
        SnarePattern snare_pattern(snare, m_section_bars);
        HatPattern hat_pattern(hat, m_section_bars);
        kick_pattern.EmitAtBar(m_sequence, m_section_start);
        snare_pattern.EmitAtBar(m_sequence, m_section_start);
        hat_pattern.EmitAtBar(m_sequence, m_section_start);

        return *this;
    }

    SectionBuilder& Kick(const std::string& pattern)
    {
        KickPattern kick_pattern(pattern, m_section_bars);
        kick_pattern.EmitAtBar(m_sequence, m_section_start);
        return *this;
    }

    SectionBuilder& Snare(const std::string& pattern)
    {
        SnarePattern snare_pattern(pattern, m_section_bars);
        snare_pattern.EmitAtBar(m_sequence, m_section_start);
        return *this;
    }

    SectionBuilder& Hat(const std::string& pattern)
    {
        HatPattern hat_pattern(pattern, m_section_bars);
        hat_pattern.EmitAtBar(m_sequence, m_section_start);
        return *this;
    }

    SectionBuilder& NoDrums()
    {
        return *this;
    }

    ////////////////////
    // CHORD PATTERNS //
    ////////////////////
    SectionBuilder& ChordRhythm(const std::string& pattern, const NoteDuration grid, const chord_fn& chord_function, const Pitch root, const int octave, const float gate = 0.9f)
    {

        const MusicalContext& context = m_sequence.GetContext();
        const float step_beats = DurationToBeats(grid, context);

        NoteSpec base;
        base.voice = VoiceType::Chord;
        base.gate = gate;

        ChordPattern chord_pattern;
        chord_pattern.pattern = pattern;
        chord_pattern.bars = m_section_bars;
        chord_pattern.step_beats = step_beats;
        chord_pattern.chord = chord_function(base, root, octave);

        chord_pattern.EmitAtBar(m_sequence, m_section_start);

        return *this;
    }

private:
    int m_bar_cursor = 0;
    int m_section_start = 0;
    int m_section_bars = 0;
    std::string m_section_name;
    SequenceBuilder& m_sequence;

    float GetSectionStartBeat() const
    {
        return static_cast<float>(m_section_start) * m_sequence.GetContext().beats_per_bar;
    }
};
