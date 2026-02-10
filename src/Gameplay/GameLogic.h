#pragma once
#include "Gameplay/GameState.h"
#include "Audio/Music/Events/EventSequence.h"
#include "Transport/MusicTransport.h"
#include "Targets/TimingTargetMode.h"

////////////////
// Game Logic //
////////////////////////////////////////////////////
// Router class to translate in-game events into  //
// appropriate game processes.                    //
// Handles input, HUD, and pool spawns            //
////////////////////////////////////////////////////
class GameLogic
{
public:
    // what happens at the start of every bar?
    static void OnBar(
        const MusicTransport& music,
        GameState& game,
        Rhythm::TimingTargetMode follow_mode);

    // what happens when a player presses ABXY?
    static void OnAction(
        InputLane lane,
        const MusicTransport& music,
        const EventSequence& sequence,
        GameState& game);

    // what happens every frame? 
    static void Update(
        const MusicTransport& music,
        const EventSequence& sequence,
        float dt_sec,
        GameState& game);
};
