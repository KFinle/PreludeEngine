#pragma once

#include "Gameplay/HUDMode.h"
#include "Transport/MusicTransport.h"
#include "Gameplay/GameState.h"
#include "Debug/PianoRollRenderer.h"
#include "Audio/Music/Events/EventSequence.h"

namespace GameplayHUD
{
    void Draw(const MusicTransport& music, GameState& game, HUDMode mode, const EventSequence& sequence, PianoRollRenderer& roll);
}
