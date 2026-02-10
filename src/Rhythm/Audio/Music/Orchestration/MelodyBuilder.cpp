// MelodyBuilder.cpp
#include "MelodyBuilder.h"
#include <vector>
#include "Audio/Music/Events/Midi.h"

///////////////
// Utilities //
///////////////
static bool IsNoteLetter(char note_letter)
{
    note_letter = static_cast<char>(std::toupper(static_cast<unsigned char>(note_letter)));
    return note_letter >= 'A' && note_letter <= 'G';
}

// pitch parsing
static bool ParsePitchToken(const std::string& tokens, Pitch& out_pitch, int& out_octave)
{
    if (tokens.size() < 2) return false;

    char letter = static_cast<char>(std::toupper(static_cast<unsigned char>(tokens[0])));
    if (!IsNoteLetter(letter)) return false;

    int index = 1;
    int semitone_offset = 0;

    // accidentals
    if (index < static_cast<int>(tokens.size()))
    {
        if (tokens[index] == '#' || tokens[index] == 's')
        {
            semitone_offset = +1;
            index++;
        }
        else if (tokens[index] == 'b')
        {
            semitone_offset = -1;
            index++;
        }
    }

    if (index >= static_cast<int>(tokens.size()) || !std::isdigit(static_cast<unsigned char>(tokens[index]))) return false;

    int octave = tokens[index] - '0';

    Pitch base_pitch;
    switch (letter)
    {
        case 'C': base_pitch = Pitch::C; break;
        case 'D': base_pitch = Pitch::D; break;
        case 'E': base_pitch = Pitch::E; break;
        case 'F': base_pitch = Pitch::F; break;
        case 'G': base_pitch = Pitch::G; break;
        case 'A': base_pitch = Pitch::A; break;
        case 'B': base_pitch = Pitch::B; break;
        default: return false;
    }

    int midi = PitchToMidi(base_pitch, octave) + semitone_offset;

    out_octave = midi / 12 - 1;
    out_pitch = MidiToPitchClass(midi);

    return true;
}

// strip whitespace and split pattern into tokens
static std::vector<std::string> Tokenize(const std::string& pattern_text)
{
    std::vector<std::string> out;
    std::string current;

    auto flush = [&]
    {
        if (!current.empty())
        {
            out.push_back(current);
            current.clear();
        }
    };

    for (char token_char : pattern_text)
    {
        if (std::isspace(static_cast<unsigned char>(token_char))) flush();
        else current.push_back(token_char);
    }
    flush();
    return out;
}

///////////////////
// MelodyBuilder //
///////////////////
namespace MelodyBuilder
{
    // emits monophonic melody from pattern
    void EmitMonophonic(SequenceBuilder& builder, const NoteSpec& base, const std::string& pattern, const Settings& settings)
    {
        const MusicalContext& context = builder.GetContext();

        // convert musical duration -> beats
        const float step = DurationToBeats(settings.step_duration, context);
        if (step <= 0.0f) return;

        // enforce bar-aligned grid
        const float beats_per_bar = static_cast<float>(context.beats_per_bar);
        const float steps_per_bar_f = beats_per_bar / step;
        const int steps_per_bar = static_cast<int>(steps_per_bar_f + 0.5f);
        if (std::fabs(steps_per_bar_f - static_cast<float>(steps_per_bar)) > 0.0001f)
        {
            // invalid grid (tuplets/swing not supported yet, but maybe if I have time)
            return;
        }

        
        const float total_beats = static_cast<float>(settings.bars) * beats_per_bar;
        float beat = settings.start_beat;
        const float end_beat = settings.start_beat + total_beats;
        bool has_pending = false;
        bool slide_pending = false;
        NoteSpec pending;
        float pending_start = 0.0f;
        float pending_duration = 0.0f;

        // helper to clear pending note
        auto flush_pending = [&]
        {
            if (!has_pending) return;
            pending.duration_beats = pending_duration;
            builder.Add(pending_start, pending);
            has_pending = false;
            pending_duration = 0.0f;
            slide_pending = false;
        };

        const auto tokens = Tokenize(pattern);

        // looping with empty pattern is invalid
        if (tokens.empty() && settings.loop_tokens) return;

        // get token at index
        auto get_token = [&](const int token_index) -> const std::string*
        {
            if (tokens.empty()) return nullptr;
            if (settings.loop_tokens) return &tokens[token_index % tokens.size()];
            if (token_index >= static_cast<int>(tokens.size())) return nullptr;
            return &tokens[token_index];
        };

        int token_index = 0;

        // main loop advances in fixed musical steps
        while (beat < end_beat)
        {
            const std::string* token_pointer = get_token(token_index);

            // out of tokens
            if (!token_pointer)
            {
                if (settings.loop_tokens)
                {
                    token_index = 0;
                    continue;
                }

                if (settings.end_mode == Settings::EndMode::Stop)
                {
                    flush_pending();
                    return;
                }
                if (settings.end_mode == Settings::EndMode::HoldFill)
                {
                    if (has_pending) pending_duration += (end_beat - beat);
                    break;
                }
                
                // fill with rests
                flush_pending();
                beat += step;
                continue;
            }
            const std::string& token = *token_pointer;
            token_index++;

            // barline (visual only; ignore)
            if (token == "|") continue;

            // rest
            if (token == "-")
            {
                flush_pending();
                beat += step;
                continue;
            }

            // hold
            if (token == ".")
            {
                if (has_pending) pending_duration += step;
                beat += step;
                continue;
            }

            // slide marker
            if (token == "~")
            {
                if (has_pending) slide_pending = true;
                continue;
            }

            // note token
            Pitch pitch_class;
            int octave = 4;

            if (!ParsePitchToken(token, pitch_class, octave))
            {
                if (settings.strict)
                {
                    flush_pending();
                    return;
                }
                flush_pending();
                beat += step;
                continue;
            }

            // start new note
            if (!has_pending)
            {
                pending = base;
                pending.pitch = pitch_class;
                pending.octave = octave;
                pending_start = beat;
                pending_duration = step;
                has_pending = true;
            }

            // slide to next pitch
            else if (slide_pending)
            {
                pending.slide_to_hz = static_cast<float>(MidiToFrequency(PitchToMidi(pitch_class, octave)));
                pending_duration += step;
                slide_pending = false;
            }
            
            // normal note change
            else
            {
                flush_pending();
                pending = base;
                pending.pitch = pitch_class;
                pending.octave = octave;
                pending_start = beat;
                pending_duration = step;
                has_pending = true;
            }
            beat += step;
        }
        flush_pending();
    }

    void EmitChord(SequenceBuilder& builder, const float start_beat, const std::vector<NoteSpec>& notes, const float duration_beats)
    {
        if (notes.empty() || duration_beats <= 0.0f) return;
        for (const NoteSpec& note_spec : notes)
        {
            NoteSpec note = note_spec;
            note.duration_beats = duration_beats;
            builder.Add(start_beat, note);
        }
    }
}
