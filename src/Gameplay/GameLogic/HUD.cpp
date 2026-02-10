#include "Gameplay/GameLogic.h"
#include "Gameplay/GameLogic/Internal.h"


bool GameLogicInternal::IsPunishableHUDMode(const HUDMode mode)
{
    return mode != HUDMode::DebugRoll;
}

void GameLogicInternal::UpdateHudTiming(const MusicTransport& music, const EventSequence& sequence, const float current_beat, GameState& game)
{
    (void)music;
    (void)sequence;

    if (!game.hud.hud_init)
    {
        game.hud.hud_anchor_beat = current_beat;
        game.hud.hud_next_beat = current_beat;
        game.hud.hud_init = true;
    }

    game.hud.render_phase = current_beat;
}

void GameLogic::OnBar(const MusicTransport&, GameState& game, const Rhythm::TimingTargetMode follow_mode)
{
    // only update gameplay while active
    if (game.gameplay.phase != GamePhase::Playing) return;

    // advance bar and adopt the new timing mode
    game.gameplay.bar_index++;
    game.gameplay.timing_mode = follow_mode;

    // reset HUD state when timing mode changes
    if (game.gameplay.timing_mode_latched != follow_mode)
    {
        game.gameplay.timing_mode_latched = follow_mode;
        game.hud.hud_init = false;

        GameLogicInternal::ResetSpawnState(game);
    }

    // clear per-bar state and clamp stability
    game.gameplay.hit_this_bar = false;
    game.ClampStability();

    // end the run if stability is empty
    if (game.gameplay.stability <= 0.0f) game.gameplay.phase = GamePhase::GameOver;
}
