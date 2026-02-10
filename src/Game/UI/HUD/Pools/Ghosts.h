#pragma once

#include <algorithm>
#include <vector>
#include "Audio/Music/Events/EventSequence.h"
#include "Audio/Music/Orchestration/NoteSpec.h"
#include "Gameplay/GameState.h"
#include "Gameplay/TimingUtils.h"
#include "Targets/TimingTargetMode.h"
#include "UI/HUD/HUDSkin.h"

////////////
// GHOSTS //
/////////////////////////////////////////////////////////////////////
// These are the notes that sit off on the side waiting their turn //
// to get on a lane. This class handles their behaviour.           //
/////////////////////////////////////////////////////////////////////
class HUDGhostPool
{
public:
    struct GhostNote
    {
        float beat = 0.0f;
        InputLane lane = InputLane::Up;
    };

    
    void Clear();
    void EnsureBuilt(const EventSequence& seq, Rhythm::TimingTargetMode mode, uint8_t active_lanes_mask);
    void Draw(const EventSequence& seq, Rhythm::TimingTargetMode mode, float current_beat, float approach_window_beats, const IHUDSkin& skin, uint8_t active_lanes_mask);
    size_t Remaining(const EventSequence& seq, Rhythm::TimingTargetMode mode, uint8_t active_lanes_mask, float current_beat);
    size_t Count() const;

private:
    void Build(const EventSequence& sequence, Rhythm::TimingTargetMode mode, uint8_t active_lanes_mask);
    bool ShouldIncludeVoice(Rhythm::TimingTargetMode mode, VoiceType voice) const;
    std::vector<float> CollectBeats(const EventSequence& seq, Rhythm::TimingTargetMode mode) const;
    void DrawLane(int lane_index, InputLane lane_id, float cutoff_beat, float approach_window_beats, const IHUDSkin& skin, size_t
                  draw_limit) const;


    static constexpr float merge_epsilon = 0.0005f;
    static constexpr float beat_bucket_scale = 1024.0f;
    static constexpr int max_draw_notes = 300;
    std::vector<GhostNote> m_notes_by_lane[4];
    const EventSequence* m_sequence = nullptr;
    Rhythm::TimingTargetMode m_mode = Rhythm::TimingTargetMode::Barline;
    uint8_t m_lanes_mask = 0;
    float m_length_beats = -1.0f;
    size_t m_note_count = 0;
};
