#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <unordered_set>
#include "Gameplay/NotePool.h"
#include "Gameplay/RunResults.h"
#include "Targets/TimingTargetMode.h"
#include "Math/MathUtils.h"
#include "Gameplay/HUDMode.h"

////////////////
// Game State //
/////////////////////////////////////////////////////////////
// This file outline all the data required for             //
// running the game. It containerizes state variables      //
// used by both the HUD and game logic.                    //
// NOTE: Most of the HUD state elements are for tooling    //
//       and debugging and may be removed or commented out //
//////////////////////////////////////////////////////////////

// GamePhase
enum class GamePhase
{
    Playing,
    GameOver
};

////////////////////
// Timing Policy  //
////////////////////

enum class TimingResult
{
    Perfect,
    Good,
    Late,
    Miss
};

struct TimingPolicy
{
    float perfect_radius_beats = 0.20f;
    float good_radius_beats = 0.50f;
    float reward_perfect = 0.20f;
    float reward_good = 0.10f;
    float penalty_late = -0.05f;
    float penalty_miss = -0.10f;

    TimingResult EvaluateBeatDelta(const float delta_beats) const
    {
        float absolute_delta = fabsf(delta_beats);
        if (absolute_delta <= perfect_radius_beats) return TimingResult::Perfect;
        if (absolute_delta <= good_radius_beats) return (delta_beats > 0.0f) ? TimingResult::Late : TimingResult::Good;
        return TimingResult::Miss;
    }

    float StabilityDelta(const TimingResult result) const
    {
        switch (result)
        {
            case TimingResult::Perfect: return reward_perfect;
            case TimingResult::Good: return reward_good;
            case TimingResult::Late: return penalty_late;
            case TimingResult::Miss: return penalty_miss;
        }
        return 0.0f;
    }
};

// timing windows for debug visuals
struct TimingHUDVisuals
{
    // float phase = 0.0f;
    // float good_start    = 0.85f;
    // float good_end      = 0.15f;
    // float perfect_start = 0.95f;
    // float perfect_end   = 0.05f;

    // debug
    // bool in_good    = false;
    // bool in_perfect = false;
};

// score tracking
struct ScoreState
{
    RunResults totals{};
    std::array<RunResults, InputLaneCount> lane_scores{};

    int TotalHits() const { return totals.perfect + totals.good + totals.late + totals.missed; }
    int CorrectHits() const { return totals.perfect + totals.good; }
};

// gameplay state
struct GameplayState
{
    // core gameplay variables
    float stability   = 1.0f;
    float decay_per_bar = 0.08f;
    int   bar_index    = 0;
    float current_beat = 0.0f;
    bool  hit_this_bar  = false;
    float cull_notes_older_than_beats = 1.0f;
    GamePhase phase = GamePhase::Playing;

    // timing target mode (what we're following this bar)
    Rhythm::TimingTargetMode timing_mode = Rhythm::TimingTargetMode::Barline;
    Rhythm::TimingTargetMode timing_mode_latched = Rhythm::TimingTargetMode::Barline;

    TimingPolicy timing_policy;

    // active notes
    GameplayPool::NotePool note_pool;

    // bucket to prevent duplicate spawns across overlapping scans
    std::unordered_set<uint32_t> spawned_beat_buckets;
    float last_spawned_beat = -INFINITY;

    // active lanes mask (up|right|down|left)
    uint8_t active_lanes_mask = 0x0F;

    // score tracking 
    ScoreState score;

    // punishment toggle
    bool punish_enabled = true;

    std::string difficulty = "Easy";
};

// visuals
struct HUDState
{
    // timing visualization
    TimingHUDVisuals timing;
    float render_phase = 0.0f;
    
    float hud_anchor_beat = 0.0f;
    float hud_next_beat   = 0.0f;
    bool  hud_init       = false;

    // player feedback
    float last_consumed_target_beat = -1.0f;
    InputLane last_hit_lane = InputLane::Up;
    float last_hit_dist_px = 0.0f;
    TimingResult last_timing = TimingResult::Miss;
    float feedback_timer = 0.0f;

    // ghost visualization
    float ghost_approach_window_beats = 6.0f;

    // hudmode
    HUDMode hud_mode = HUDMode::SinglePlayer;

    // active lanes mask (up|right|down|left)
    uint8_t active_lanes_mask = 0x0F;
};

// combined gamestate
struct GameState
{
    GameplayState gameplay;
    HUDState hud;

    void ClampStability()
    {
        gameplay.stability = ClampFloat(gameplay.stability, 0.0f, 1.0f);
    }

    void ResetRunState()
    {
        *this = GameState{};
    }
};
