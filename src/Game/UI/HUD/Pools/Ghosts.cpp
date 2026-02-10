#include "Ghosts.h"

// implementation
void HUDGhostPool::Clear()
{
    for (auto& lane : m_notes_by_lane)
    {
        lane.clear();
    }
    
    m_sequence = nullptr;
    m_length_beats = -1.0f;
    m_note_count = 0;
}

void HUDGhostPool::EnsureBuilt(const EventSequence& seq, const Rhythm::TimingTargetMode mode, const uint8_t active_lanes_mask)
{
    const float length = seq.GetLengthBeats();
    const size_t count = seq.notes.size();

    const bool needs_rebuild = (m_sequence != &seq || m_mode != mode || 
        m_lanes_mask != active_lanes_mask ||
        fabsf(length - m_length_beats) > 0.001f || 
        count != m_note_count);
    
    if (needs_rebuild) Build(seq, mode, active_lanes_mask);
}

void HUDGhostPool::Draw(const EventSequence& seq, const Rhythm::TimingTargetMode mode, const float current_beat,
                        const float approach_window_beats, const IHUDSkin& skin, const uint8_t active_lanes_mask)
{
    EnsureBuilt(seq, mode, active_lanes_mask);
    if (Count() == 0) return;

    const int active_lane_count = ActiveLaneCount(active_lanes_mask);
    if (active_lane_count <= 0) return;

    const float cutoff_beat = current_beat + approach_window_beats;
    const size_t per_lane_limit = MaxFloat(1u, max_draw_notes / active_lane_count);

    for (int lane_idx = 0; lane_idx < 4; ++lane_idx)
    {
        const InputLane lane_id = static_cast<InputLane>(lane_idx);
        if (IsLaneActive(active_lanes_mask, lane_id)) DrawLane(lane_idx, lane_id, cutoff_beat, approach_window_beats, skin, per_lane_limit);
    }
}

size_t HUDGhostPool::Remaining(const EventSequence& seq, const Rhythm::TimingTargetMode mode,
                               const uint8_t active_lanes_mask, const float current_beat)
{
    EnsureBuilt(seq, mode, active_lanes_mask);

    size_t total = 0;
    for (const auto& notes : m_notes_by_lane)
    {
        const auto it = std::lower_bound(notes.begin(), notes.end(), current_beat,
                                         [](const GhostNote& note, const float beat) { return note.beat < beat; });
        total += std::distance(it, notes.end());
    }

    return total;
}

size_t HUDGhostPool::Count() const
{
    size_t total = 0;
    for (const auto& lane : m_notes_by_lane)
    {
        total += lane.size();
    }
    return total;
}

void HUDGhostPool::Build(const EventSequence& sequence, const Rhythm::TimingTargetMode mode, const uint8_t active_lanes_mask)
{
    for (auto& lane : m_notes_by_lane) lane.clear();

    m_sequence = &sequence;
    m_mode = mode;
    m_lanes_mask = active_lanes_mask;
    m_length_beats = sequence.GetLengthBeats();
    m_note_count = sequence.notes.size();

    const std::vector<float> beats = CollectBeats(sequence, mode);

    for (const float beat : beats)
    {
        GhostNote ghost;
        ghost.beat = beat;
        ghost.lane = GetLaneForBeat(beat, active_lanes_mask, beat_bucket_scale);

        const int lane_idx = static_cast<int>(ghost.lane);
        if (lane_idx >= 0 && lane_idx < 4) m_notes_by_lane[lane_idx].push_back(ghost);
    }
}

bool HUDGhostPool::ShouldIncludeVoice(const Rhythm::TimingTargetMode mode, const VoiceType voice) const
{
    switch (mode)
    {
    case Rhythm::TimingTargetMode::Snare: return voice == VoiceType::Snare;
    case Rhythm::TimingTargetMode::Kick: return voice == VoiceType::Kick;
    case Rhythm::TimingTargetMode::KickAndSnare: return voice == VoiceType::Kick || voice == VoiceType::Snare;
    case Rhythm::TimingTargetMode::Melody: return voice == VoiceType::Lead;
    case Rhythm::TimingTargetMode::All:
        return voice == VoiceType::Lead || voice == VoiceType::Kick ||
            voice == VoiceType::Snare || voice == VoiceType::Hat ||
            voice == VoiceType::Triangle || voice == VoiceType::Chord;
    case Rhythm::TimingTargetMode::Barline: break;
    }
    return false;
}

std::vector<float> HUDGhostPool::CollectBeats(const EventSequence& seq, const Rhythm::TimingTargetMode mode) const
{
    std::vector<float> beats;

    if (mode == Rhythm::TimingTargetMode::Barline)
    {
        const float beats_per_bar = static_cast<float>(seq.beats_per_bar);
        const int bar_count = static_cast<int>(ceilf(m_length_beats / MaxFloat(1.0f, beats_per_bar))) + 1;
        beats.reserve(bar_count);

        for (int bar = 0; bar < bar_count; ++bar)
        {
            beats.push_back(static_cast<float>(bar) * beats_per_bar);
        }
    }
    else
    {
        beats.reserve(seq.notes.size());
        for (const auto& note : seq.notes)
        {
            if (ShouldIncludeVoice(mode, note.voice)) beats.push_back(note.start_beat);
        }
    }

    // remove duplicates
    std::sort(beats.begin(), beats.end());
    beats.erase(std::unique(beats.begin(), beats.end(),
                            [](const float a, const float b) { return fabsf(a - b) < merge_epsilon; }),
                beats.end());
    return beats;
}

void HUDGhostPool::DrawLane(const int lane_index, const InputLane lane_id,
                            const float cutoff_beat, const float approach_window_beats,
                            const IHUDSkin& skin, const size_t draw_limit) const
{
    const auto& notes = m_notes_by_lane[lane_index];
    if (notes.empty()) return;

    // get lane geometry
    float anchor_x, anchor_y, lane_end_x, lane_end_y;
    skin.SetGhostPathOrigin(lane_id, anchor_x, anchor_y);
    skin.SetLaneEnd(lane_id, lane_end_x, lane_end_y);

    const float dx = lane_end_x - anchor_x;
    const float dy = lane_end_y - anchor_y;
    const float base_distance = sqrtf(dx * dx + dy * dy);
    const float base_angle = (base_distance < 0.001f) ? GetLaneAngle(lane_id) : atan2f(dy, dx);
    const float inner_radius = (base_distance < 0.001f) ? 1.0f : base_distance;
    const float outer_radius = inner_radius + MaxFloat(1.0f, skin.GetGhostTravelDistance());

    // animation windows
    const float travel_window = MaxFloat(1.0f, approach_window_beats * 0.5f);
    const float fade_window = MaxFloat(0.5f, travel_window);

    // find first note to draw
    const auto first_note = std::upper_bound(notes.begin(), notes.end(), cutoff_beat,
                                             [](const float beat, const GhostNote& note) { return beat < note.beat; });

    // draw notes
    size_t drawn = 0;
    for (auto it = first_note; it != notes.end() && drawn < draw_limit; ++it)
    {
        const float time_to_spawn = it->beat - cutoff_beat;
        if (time_to_spawn < 0.0f) continue;

        // calculate fade and travel
        const float fade = ClampFloat(time_to_spawn / fade_window, 0.0f, 1.0f);
        const float alpha = 0.45f / fade;
        
        float travel_progress = 0.0f;
        if (time_to_spawn < travel_window) travel_progress = 1.0f - (time_to_spawn / travel_window);
        travel_progress = SmoothStep(ClampFloat(travel_progress, 0.0f, 1.0f));

        // add visual variation
        const uint32_t bucket = Rhythm::BeatBucket(it->beat, beat_bucket_scale);
        const float jitter = (static_cast<int>(bucket % 9) - 4) * 2.0f;
        const float angle_offset = (static_cast<int>((bucket / 7) % 7) - 3) * 0.05f;

        // calculate position
        const float radius = LerpFloat(outer_radius + jitter, inner_radius, travel_progress);
        const float angle = base_angle + angle_offset * (1.0f - travel_progress);
        const float x = anchor_x + cosf(angle) * radius;
        const float y = anchor_y + sinf(angle) * radius;

        // draw
        const float size = LerpFloat(2.0f, 3.0f, travel_progress);
        const float depth = ClampFloat((radius - inner_radius) / MaxFloat(0.001f, outer_radius - inner_radius), 0.0f, 1.0f);
        
        skin.DrawGhost(x, y, it->lane, depth, alpha, size);
        ++drawn;
    }
}
