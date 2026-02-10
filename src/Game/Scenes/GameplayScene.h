#pragma once

#include <string>
#include "Scenes/Scene.h"
#include "Scenes/SceneManager.h"
#include "Music/MusicClipManager.h"
#include "Debug/PianoRollRenderer.h"
#include "Audio/Music/Events/EventSequence.h"
#include "Gameplay/GameState.h"
#include "Gameplay/HUDMode.h"
#include "Music/AsyncSongRender.h"
#include "Transport/MusicTransport.h"
#include "Targets/TimingTargetMode.h"

class GameplayScene final : public Scene
{
public:
    GameplayScene();
    void OnEnter(SceneManager& manager) override;
    void OnExit(SceneManager& manager) override;
    void Update(float dt_sec) override;
    void Render() override;

private:
    ////////////////////
    // Gameplay state //
    ////////////////////
    GameMode   m_game_mode{};
    GameState  m_game{};
    HUDMode    m_hud_mode{HUDMode::SinglePlayer};

    bool m_playing = false;
    int  m_last_bar = -1;

    /////////// 
    // Music //
    /////////// 
    MusicClipManager m_music;
    EventSequence m_seq;
    PianoRollRenderer m_roll;
    MusicTransport m_music_time;
    std::string m_song_id;
    Rhythm::TimingTargetMode m_voice_follow = Rhythm::TimingTargetMode::Kick;

    //////////////////
    // Async render //
    //////////////////
    AsyncSongRender m_async_render;
    bool m_song_ready = false;

    std::string GetGameModeString()
    {
        return
        m_game_mode == GameMode::Easy ? "Easy" :
        m_game_mode == GameMode::Medium ? "Medium" :
        m_game_mode == GameMode::Hard ? "Hard" :
        m_game_mode == GameMode::Brutal ? "Brutal" :
        m_game_mode == GameMode::Test ? "Test" : "Unknown";
    }
};
