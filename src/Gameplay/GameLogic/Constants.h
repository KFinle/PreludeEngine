#pragma once
#include <cmath>
namespace GLC
{
    static constexpr int lane_count = 4;
    static constexpr float target_merge_eps_beats  = 0.0005f;
    static constexpr float beat_bucket_scale     = 1024.0f;
    static constexpr float neg_inf_beat = -INFINITY;
    static constexpr float pos_inf_beat =  INFINITY;
    static constexpr float min_interval_beats = 0.0001f;
    static constexpr float min_spacing_beats  = 0.0010f;
    static constexpr float hud_back_horizon_beats = 16.0f;
    static constexpr float hud_fwd_horizon_beats  = 32.0f;
    static constexpr float spawn_advance_eps_beats = 0.0010f;
    static constexpr float entity_early_cull_beats = -0.25f;
    static constexpr float feedback_duration_sec = 1.0f;
    static constexpr float entity_approach_window_beats = 4.0f;
    static constexpr float entity_max_dist_px_ratio = 0.62f;
    static constexpr float entity_min_dist_px = 0.0f;
    static constexpr float spawn_bucket_keep_back_beats = 8.0f;
    static constexpr float spawn_bucket_keep_fwd_beats  = 8.0f;
}
