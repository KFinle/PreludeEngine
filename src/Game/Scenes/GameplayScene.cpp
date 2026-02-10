#include "GameplayScene.h"
#include "SceneManager.h"
#include "Engine/Engine.h"
#include "Audio/Music/Orchestration/GameplaySongs.h"
#include "Audio/Music/Orchestration/TestSequences.h"
#include "Debug/DebugLogger.h"
#include "Gameplay/GameLogic.h"
#include "Gameplay/RunResults.h"
#include "UI/Core/GameUI.h"
#include "UI/HUD/GameplayHUD.h"

GameplayScene::GameplayScene() = default;

void GameplayScene::OnEnter(SceneManager& manager)
{
    // scene init 
    Scene::OnEnter(manager);
    m_game_mode = manager.GetCurrentMode();
    Logger::PrintLog(Logger::GAME, "Entering GameplayScene");
    Logger::PrintLog(Logger::GAME, "Current mode: " + GetGameModeString());
    

    // TODO:
    // If there's time, I should come back 
    // and make specific mixes for the songs 
    MixSettings mix{};

    ////////////////////
    // GameState init //
    ////////////////////
    m_game = GameState{};
    m_game.gameplay.stability = 1.0f;  
    m_game.gameplay.punish_enabled = !Scene::no_death_mode;
    m_playing = false;
    m_hud_mode = Scene::two_player_mode ? HUDMode::TwoPlayer : HUDMode::SinglePlayer;
    m_last_bar = -1;

    //////////////////// 
    // Song Selection //
    //////////////////// 
    switch (m_game_mode)
    {
        case GameMode::Easy:
            m_song_id = "Easy";
            m_seq = MakeSong_Easy(mix);
            break;
        case GameMode::Medium:
            m_song_id = "Medium";
            m_seq = MakeSong_Medium(mix);
            break;
        case GameMode::Hard:
            m_song_id = "Hard";
            m_seq = MakeSong_Hard(mix);
            break;
        case GameMode::Brutal:
            m_song_id = "Brutal";
            m_seq = MakeSong_Brutal(mix);
            break;
        case GameMode::Test:
            m_song_id = "Test";

            // tests for the composition API
            //m_seq = MakeSong_CompositionAPITests(mix);

            // note density test
            m_game.gameplay.cull_notes_older_than_beats = 200.0f;
            m_voice_follow = Rhythm::TimingTargetMode::All;
            mix.lead_gain = 0.03f;
            m_seq = MakeSong_NotePoolTest(mix);
            m_game.gameplay.punish_enabled = false;
            m_hud_mode = HUDMode::DebugRoll;
            break;
    }
    
    m_game.gameplay.difficulty = m_song_id;
    
    // begin rendering song to WAV
    m_async_render.Start(&m_music, m_song_id, m_seq);
    m_song_ready = false;

    // setup MusicTransport
    m_music_time.bpm = m_seq.bpm;
    m_music_time.beats_per_bar = m_seq.beats_per_bar;
    m_music_time.Reset();
}

void GameplayScene::OnExit(SceneManager& manager)
{
    if (m_playing) m_music.Stop(m_song_id);

    // ensure async render is finished
    m_async_render.Join();
    m_music_time.Reset();

    // scene cleanup
    m_playing = false;
    m_music.CleanupGeneratedFiles();
    m_seq = EventSequence{};

    Logger::PrintLog(Logger::GAME, "Exiting GameplayScene");
    Scene::OnExit(manager);
}

void GameplayScene::Update(const float dt_sec)
{
    if (!m_scenemanager) return;

    // load song
    if (!m_song_ready)
    {
        if (m_async_render.IsDone())
        {
            if (m_async_render.Succeeded())
            {
                m_song_ready = true;
                m_music_time.Reset();
                m_music.Play(m_song_id, false);
                m_playing = true;
                m_last_bar = -1;
                Logger::PrintLog(Logger::GAME, "Song render complete");
            }
            else
            {
                Logger::PrintLog(Logger::GAME, "Song render failed: " + m_async_render.GetError());
                m_scenemanager->Request(SceneType::Intro);
            }
        }
        if (Engine::GetController().CheckButton(Engine::BTN_BACK, true)) m_scenemanager->Request(SceneType::Intro);
        
        return;
    }

    // normal gameplay
    if (m_playing)
    {
        m_music_time.Update(dt_sec);
        GameLogic::Update(m_music_time, m_seq, dt_sec, m_game);

        if (m_music_time.raw_seconds > m_seq.GetLengthSec())
        {
            m_playing = false;
            m_game.gameplay.phase = GamePhase::GameOver;
        }
    }

    // removed because the song should just start when rendering is done
    // if (Engine::GetController().CheckButton(Engine::BTN_LBUMPER, true) && !m_playing)
    // {
    //     m_music_time.Reset();
    //     m_music.Play(m_song_id, false);
    //     m_playing = true;
    //     m_last_bar = -1;
    // }

    // exit 
    if (Engine::GetController().CheckButton(Engine::BTN_RBUMPER, true))
    {
        m_scenemanager->Request(SceneType::Intro);
    }

    // cycle huds
    if (Engine::GetController().CheckButton(Engine::BTN_DPAD_RIGHT, true))
    {
        m_hud_mode = static_cast<HUDMode>((static_cast<int>(m_hud_mode) + 1) % NumHudModes);
    }

    // skip to debug piano roll
    if (Engine::GetController().CheckButton(Engine::BTN_DPAD_DOWN, true))
    {
        m_hud_mode = HUDMode::DebugRoll;
    }

    if (m_playing)
    {
        // gameplay input
        auto& pad = Engine::GetController();
        if (pad.CheckButton(Engine::BTN_X, true) || Engine::WasKeyPressed(Engine::KEY_A))
        {
            GameLogic::OnAction(InputLane::Left,  m_music_time, m_seq, m_game);
        }
        if (pad.CheckButton(Engine::BTN_A, true) || Engine::WasKeyPressed(Engine::KEY_S))
        {
            GameLogic::OnAction(InputLane::Down,  m_music_time, m_seq, m_game);
        }
        if (pad.CheckButton(Engine::BTN_B, true) || Engine::WasKeyPressed(Engine::KEY_D))
        {
            GameLogic::OnAction(InputLane::Right, m_music_time, m_seq, m_game);
        }
        if (pad.CheckButton(Engine::BTN_Y, true) || Engine::WasKeyPressed(Engine::KEY_W))
        {
            GameLogic::OnAction(InputLane::Up,    m_music_time, m_seq, m_game);
        }
    }

    const int bar_index = m_music_time.GetBarIndex();
    if (m_playing && bar_index != m_last_bar)
    {
        GameLogic::OnBar(m_music_time, m_game, m_voice_follow);
        m_last_bar = bar_index;
    }

    if (m_game.gameplay.phase == GamePhase::GameOver)
    {
        if (m_playing)
        {
            m_music.Stop(m_song_id);
            m_playing = false;
        }

        // two player mode
        run_results_p2 = RunResults{};
        if (two_player_mode)
        {
            const int left_lane_index = GetLaneIndex(InputLane::Left);
            const int right_lane_index = GetLaneIndex(InputLane::Right);
            run_results = m_game.gameplay.score.lane_scores[left_lane_index];
            run_results_p2 = m_game.gameplay.score.lane_scores[right_lane_index];
        }
        else
        {
            run_results = m_game.gameplay.score.totals;
        }
        
        m_scenemanager->Request(SceneType::End);
    }
}

void GameplayScene::Render()
{
    if (!m_song_ready)
    {
        GameUI::PrintCenteredText("Rendering music...", true);
    }
    
    else GameplayHUD::Draw(m_music_time, m_game, m_hud_mode, m_seq, m_roll);
}
