#include "Gameplay/GameLogic.h"
#include "Gameplay/GameLogic/Constants.h"
#include "Gameplay/GameLogic/Internal.h"
#include "Targets/TimingTargets.h"
#include "Gameplay/TimingUtils.h"
#include "Math/MathUtils.h"
#include "Config/AppConfig.h"
#include <algorithm>
#include <vector>

//////////////////////
// Spawn Controller //
/////////////////////////////////////////////////////////////
// Handles how the game spawns notes to be interacted with //
/////////////////////////////////////////////////////////////
namespace
{
    // map timing mode to audio voice
    VoiceType VoiceForMode(const Rhythm::TimingTargetMode mode)
    {
        switch (mode)
        {
            case Rhythm::TimingTargetMode::Kick:  return VoiceType::Kick;
            case Rhythm::TimingTargetMode::Snare: return VoiceType::Snare;
            default:                      return VoiceType::Lead;
        }
    }

    void DeduplicateTargets(std::vector<float>& targets)
    {
        std::sort(targets.begin(), targets.end());
        targets.erase(std::unique(targets.begin(), targets.end(),
            [](const float left, const float right)
            {
                return fabsf(left - right) < GLC::target_merge_eps_beats;
            }), targets.end());
    }

    // convert approach window to note speed
    float CalculateNoteSpeed(const MusicTransport& music, const float approach_window_beats, const float max_dist_px)
    {
        const float window_sec = BeatsToSeconds(approach_window_beats, music.bpm);
        return (max_dist_px - GLC::entity_min_dist_px) / MaxFloat(0.0001f, window_sec);
    }

    float ComputeInitialDistance(const MusicTransport& music,
                                 const float current_beat,
                                 const float target_beat,
                                 const float approach_window_beats,
                                 const float max_dist_px,
                                 const float speed_px_per_sec)
    {
        const float approach_start_beat = target_beat - approach_window_beats;
        
        if (current_beat <= approach_start_beat) return max_dist_px;

        const float beats_since_start = current_beat - approach_start_beat;
        const float sec_since_start = BeatsToSeconds(beats_since_start, music.bpm);
        const float distance_traveled = speed_px_per_sec * sec_since_start;
        return ClampFloat(max_dist_px - distance_traveled, GLC::entity_min_dist_px, max_dist_px);
    }

    // prune spawn buckets outside the window
    void CleanupSpawnedBeatBuckets(GameState& game, const float current_beat)
    {
        const float min_beat = MaxFloat(0.0f, current_beat - GLC::spawn_bucket_keep_back_beats);
        const float max_beat = current_beat + GLC::spawn_bucket_keep_fwd_beats;

        const uint32_t min_bucket = Rhythm::BeatBucket(min_beat, GLC::beat_bucket_scale);
        const uint32_t max_bucket = Rhythm::BeatBucket(max_beat, GLC::beat_bucket_scale);

        auto& buckets = game.gameplay.spawned_beat_buckets;
        for (auto iterator = buckets.begin(); iterator != buckets.end();)
        {
            const uint32_t bucket = *iterator;
            if (bucket < min_bucket || bucket > max_bucket) iterator = buckets.erase(iterator);
            else ++iterator;
        }
    }

    // spawn pass for upcoming beats
    void SpawnNotes(
        const MusicTransport& music,
        const EventSequence& sequence,
        const Rhythm::TimingTargetMode mode,
        const float current_beat,
        GameState& game)
    {
        // determine the spawn window for this frame
        const float approach_window_beats = GLC::entity_approach_window_beats;
        const float end_beat = current_beat + approach_window_beats;

        float start_beat = game.gameplay.last_spawned_beat;
        if (start_beat < current_beat) start_beat = current_beat;

        // collect targets to spawn in this horizon
        const float horizon = end_beat - start_beat;
        if (horizon <= 0.0f) return;

        std::vector<float> targets;
        Rhythm::CollectTimingTargets(sequence, mode, start_beat, horizon, targets);

        if (targets.empty())
        {
            game.gameplay.last_spawned_beat = end_beat + GLC::spawn_advance_eps_beats;
            return;
        }

        // remove near-duplicate targets
        DeduplicateTargets(targets);

        // compute spawn parameters shared by all targets
        const VoiceType voice = VoiceForMode(mode);
        const float max_dist_px = APP_VIRTUAL_HEIGHT * GLC::entity_max_dist_px_ratio;
        const float speed_px_per_sec = CalculateNoteSpeed(music, approach_window_beats, max_dist_px);

        for (float target_beat : targets)
        {
            // skip targets too far behind
            if (target_beat < current_beat + GLC::entity_early_cull_beats) continue;

            // de-dupe by beat bucket
            const uint32_t bucket = Rhythm::BeatBucket(target_beat, GLC::beat_bucket_scale);
            if (game.gameplay.spawned_beat_buckets.count(bucket)) continue;

            game.gameplay.spawned_beat_buckets.insert(bucket);

            // compute initial position based on current beat
            const float initial_dist_px = ComputeInitialDistance(music, current_beat, target_beat,
                                                                 approach_window_beats, max_dist_px, speed_px_per_sec);

            // select a lane and spawn the note
            const InputLane lane = GetLaneForBeat(target_beat, game.gameplay.active_lanes_mask, GLC::beat_bucket_scale);

            game.gameplay.note_pool.SpawnNote(
                target_beat,
                lane,
                voice,
                1.0f,
                initial_dist_px,
                speed_px_per_sec);
        }

        // advance the spawn cursor beyond the window
        game.gameplay.last_spawned_beat = end_beat + GLC::spawn_advance_eps_beats;
    }

    // update UI feedback timers
    void UpdateFeedbackTimer(const float dt_sec, GameState& game)
    {
        if (game.hud.feedback_timer > 0.0f) game.hud.feedback_timer = MaxFloat(0.0f, game.hud.feedback_timer - dt_sec);
    }
}

void GameLogicInternal::ResetSpawnState(GameState& game)
{
    game.gameplay.note_pool.Clear();
    game.gameplay.spawned_beat_buckets.clear();
    game.gameplay.last_spawned_beat = GLC::neg_inf_beat;
    game.gameplay.hit_this_bar = false;
}

void GameLogic::Update(const MusicTransport& music, const EventSequence& sequence, const float dt_sec, GameState& game)
{
    // update UI feedback timers
    UpdateFeedbackTimer(dt_sec, game);

    // react to any lane mask changes
    const uint8_t desired_mask = game.hud.active_lanes_mask;
    
    if (desired_mask != game.gameplay.active_lanes_mask)
    {
        game.gameplay.active_lanes_mask = desired_mask;
        GameLogicInternal::ResetSpawnState(game);
    }

    // cache current beat for this frame
    const float current_beat = music.GetBeat();
    game.gameplay.current_beat = current_beat;
    game.hud.ghost_approach_window_beats = GLC::entity_approach_window_beats;

    // spawn, move, and cull notes
    SpawnNotes(music, sequence, game.gameplay.timing_mode, current_beat, game);

    game.gameplay.note_pool.StepMotion(dt_sec, GLC::entity_min_dist_px);

    const float miss_window_beats = game.gameplay.timing_policy.good_radius_beats;
    GameLogicInternal::ProcessMissedNotes(current_beat, miss_window_beats, game);

    game.gameplay.note_pool.CullByBeat(current_beat, game.gameplay.cull_notes_older_than_beats);
    CleanupSpawnedBeatBuckets(game, current_beat);

    // clamp stability and end the run if needed
    game.ClampStability();

    if (game.gameplay.stability <= 0.0f) game.gameplay.phase = GamePhase::GameOver;

    // update HUD timing visuals
    GameLogicInternal::UpdateHudTiming(music, sequence, current_beat, game);
}
