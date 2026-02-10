#include "Targets/TimingTargets.h"
#include "Audio/Music/Events/EventSequence.h"
#include "Audio/Music/Orchestration/NoteSpec.h"

#include <cmath>

// figures out what notes in the EventSequence needs to have a gameplay entity attached to them
namespace Rhythm
{
void CollectTimingTargets(const EventSequence& seq, const TimingTargetMode mode, const float current_beat, const float lookahead_beats, std::vector<float>& out_targets)
{
    out_targets.clear();
    static constexpr float lookback_beats = 0.5f;
    const float start = current_beat - lookback_beats;
    const float end = current_beat + lookahead_beats;

    // barline targets
    if (mode == TimingTargetMode::Barline)
    {
        const float beats_per_bar = static_cast<float>(seq.beats_per_bar);
        const int start_bar = static_cast<int>(floorf(start / beats_per_bar));
        const int end_bar = static_cast<int>(ceilf(end / beats_per_bar));

        for (int bar_index = start_bar; bar_index <= end_bar; ++bar_index)
        {
            out_targets.push_back(static_cast<float>(bar_index) * beats_per_bar);
        }
        return;
    }

    // note targets
    auto included_voice_target = [&](const VoiceType voice)
    {
        switch (mode)
        {
            case TimingTargetMode::Snare: return voice == VoiceType::Snare;
            case TimingTargetMode::Kick: return voice == VoiceType::Kick;
            case TimingTargetMode::KickAndSnare: return voice == VoiceType::Kick || voice == VoiceType::Snare;
            case TimingTargetMode::Melody: return voice == VoiceType::Lead;
            case TimingTargetMode::All:  return voice == VoiceType::Lead ||
                                                voice == VoiceType::Kick ||
                                                voice == VoiceType::Snare ||
                                                voice == VoiceType::Hat ||
                                                voice == VoiceType::Triangle ||
                                                voice == VoiceType::Chord;
            default:
                return false;
        }
    };

    for (const auto& note_event : seq.notes)
    {
        if (note_event.start_beat < start || note_event.start_beat > end) continue;
        if (included_voice_target(note_event.voice)) out_targets.push_back(note_event.start_beat);
    }
}
}
