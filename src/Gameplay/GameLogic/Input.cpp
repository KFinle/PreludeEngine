#include "Gameplay/GameLogic.h"

#include "Gameplay/GameLogic/Constants.h"
#include "Gameplay/GameLogic/Internal.h"
#include "Gameplay/TimingUtils.h"

//////////////////////
// Input Processing //
/////////////////////////////////////////////////
// Handles how the game reacts to player input //
/////////////////////////////////////////////////

namespace
{
    void UpdateResults(RunResults& results, const TimingResult result)
    {
        switch (result)
        {
            case TimingResult::Perfect: results.perfect++; break;
            case TimingResult::Good: results.good++; break;
            case TimingResult::Late: results.late++; break;
            case TimingResult::Miss: results.missed++; break;
            default: break;
        }
    }

    void RegisterHit(GameState& game, const TimingResult result, const InputLane lane)
    {
        UpdateResults(game.gameplay.score.totals, result);

        const int lane_index = GetLaneIndex(lane);
        if (lane_index >= 0 && lane_index < static_cast<int>(game.gameplay.score.lane_scores.size()))
        {
            UpdateResults(game.gameplay.score.lane_scores[static_cast<size_t>(lane_index)], result);
        }
    }

    void ApplyTimingFeedback(GameState& game, const TimingResult result, const InputLane lane)
    {
        game.hud.last_timing = result;
        game.hud.feedback_timer  = GLC::feedback_duration_sec;
        RegisterHit(game, result, lane);
    }

    void ApplyStabilityDelta(GameState& game, const TimingResult result, const bool clamp_after)
    {
        if (!game.gameplay.punish_enabled) return;

        const float delta = game.gameplay.timing_policy.StabilityDelta(result);
        game.gameplay.stability += delta;
        if (clamp_after) game.ClampStability();
    }
}

void GameLogicInternal::ProcessMissedNotes(const float current_beat, const float miss_window_beats, GameState& game)
{
    for (size_t note_index = 0; note_index < game.gameplay.note_pool.Count(); ++note_index)
    {
        auto& note = game.gameplay.note_pool.GetNote(note_index);
        if (note.consumed) continue;

        if (current_beat > note.beat + miss_window_beats)
        {
            if (!note.miss_registered)
            {
                note.miss_registered = true;
                RegisterHit(game, TimingResult::Miss, note.lane);
                if (game.gameplay.punish_enabled && IsPunishableHUDMode(game.hud.hud_mode))
                {
                    const float delta = game.gameplay.timing_policy.StabilityDelta(TimingResult::Miss);
                    game.gameplay.stability += delta;
                    game.ClampStability();
                }
            }
            note.consumed = true;
        }
    }
}

void GameLogic::OnAction(const InputLane lane, const MusicTransport& music, const EventSequence&, GameState& game)
{
    // ignore input outside active gameplay
    if (game.gameplay.phase != GamePhase::Playing) return;
    if (!IsLaneActive(game.gameplay.active_lanes_mask, lane)) return;

    // find the closest note on this lane
    const float current_beat = music.GetBeat();

    int best_note_index  = -1;
    float best_distance = GLC::pos_inf_beat;

    for (size_t note_index = 0; note_index < game.gameplay.note_pool.Count(); ++note_index)
    {
        const auto& note = game.gameplay.note_pool.GetNote(note_index);
        if (note.consumed || note.lane != lane) continue;

        const float distance = fabsf(current_beat - note.beat);
        if (distance < best_distance)
        {
            best_distance = distance;
            best_note_index = static_cast<int>(note_index);
        }
    }

    // register a miss if no target exists
    if (best_note_index < 0)
    {
        ApplyTimingFeedback(game, TimingResult::Miss, lane);
        ApplyStabilityDelta(game, TimingResult::Miss, true);
        return;
    }

    // evaluate the timing window for the best candidate
    auto& note = game.gameplay.note_pool.GetNote(static_cast<size_t>(best_note_index));
    const TimingResult result = game.gameplay.timing_policy.EvaluateBeatDelta(current_beat - note.beat);

    if (result == TimingResult::Miss)
    {
        ApplyTimingFeedback(game, TimingResult::Miss, lane);
        ApplyStabilityDelta(game, TimingResult::Miss, false);
        return;
    }

    // consume the note and record the hit
    note.consumed = true;
    game.gameplay.hit_this_bar = true;
    game.hud.last_consumed_target_beat = note.beat;
    game.hud.last_hit_lane = lane;
    game.hud.last_hit_dist_px = note.distance_px;

    ApplyTimingFeedback(game, result, lane);
    ApplyStabilityDelta(game, result, false);
}
