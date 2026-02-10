#pragma once
#include "Gameplay/GameState.h"
#include "Gameplay/HUDMode.h"
#include "Transport/MusicTransport.h"
#include "Audio/Music/Events/EventSequence.h"

// small function wrapper for GameLogic
namespace GameLogicInternal
{
    void ResetSpawnState(GameState& game);
    void ProcessMissedNotes(float current_beat, float miss_window_beats, GameState& game);
    void UpdateHudTiming(const MusicTransport& music, const EventSequence& sequence, float current_beat, GameState& game);
    bool IsPunishableHUDMode(HUDMode mode);
}
