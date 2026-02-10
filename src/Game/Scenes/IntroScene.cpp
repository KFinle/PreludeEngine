#include "IntroScene.h"
#include "UI/Core/GameUI.h"
#include "Engine/Engine.h"
#include "Scenes/SceneManager.h"
#include <cmath>

IntroScene::IntroScene() = default;

void IntroScene::Update(const float delta_sec)
{
    if (!m_scenemanager) return;

    if (m_intro_countdown >= 0.0f && !m_sound_warning_triggered)
    {
        m_intro_countdown -= delta_sec;
    }
    else
    {
        m_sound_warning_triggered = true;

        if (Engine::GetController().CheckButton(Engine::BTN_A, true) || Engine::WasKeyPressed(Engine::KEY_S))
        {
            m_scenemanager->Request(SceneType::Gameplay, GameMode::Brutal);
        }
        else if (Engine::GetController().CheckButton(Engine::BTN_B, true) || Engine::WasKeyPressed(Engine::KEY_D))
        {
            m_scenemanager->Request(SceneType::Gameplay, GameMode::Hard);
        }
        else if (Engine::GetController().CheckButton(Engine::BTN_X, true) || Engine::WasKeyPressed(Engine::KEY_A))
        {
            m_scenemanager->Request(SceneType::Gameplay, GameMode::Easy);
        }
        else if (Engine::GetController().CheckButton(Engine::BTN_Y, true) || Engine::WasKeyPressed(Engine::KEY_W))
        {
            m_scenemanager->Request(SceneType::Gameplay, GameMode::Medium);
        }
        else if (Engine::GetController().CheckButton(Engine::BTN_RBUMPER, true) || Engine::WasKeyPressed(Engine::KEY_R))
        {
            m_scenemanager->Request(SceneType::Gameplay, GameMode::Test);
        }
        else if (Engine::GetController().CheckButton(Engine::BTN_LBUMPER, true) || Engine::WasKeyPressed(Engine::KEY_E))
        {
            no_death_mode = !no_death_mode;      
        }
        else if (Engine::GetController().CheckButton(Engine::BTN_BACK, true) || Engine::WasKeyPressed(Engine::KEY_TAB))
        {
            two_player_mode = !two_player_mode;
        }
    }
}

void IntroScene::Render()
{
    static float phase = 0.0f;
    phase += 0.01f;

    if (!m_sound_warning_triggered)
    {
        float pulse = (sinf(phase * 2.0f) + 1.0f) * 0.5f;
        float red = 0.3f + 0.2f * pulse;
        float green = 0.5f + 0.3f * pulse;
        float blue = 0.9f + 0.1f * pulse;

        GameUI::PrintCenteredText("SOUND ON! This is a rhythm game :)", true, red, green, blue);
    }
    else
    {
        const float scale = 2.0f;
        const int layout_y = APP_VIRTUAL_HEIGHT / 2;

        DrawMenu(APP_VIRTUAL_WIDTH / 2, layout_y, scale);
        DrawHowToPlay(40, APP_VIRTUAL_HEIGHT - 30);
    }
}

void IntroScene::DrawMenu(const float center_x, const float center_y, const float scale)
{
    const std::string mode_label = "SELECT DIFFICULTY";
    const float label_width = mode_label.length() * 10;
    Engine::Print(center_x - label_width / 2, center_y - 180 * scale, mode_label.c_str(), 1.0f, 1.0f, 1.0f);

    const float button_radius = 20 * scale;
    const float button_spacing = 80 * scale;
    const float layout_y = center_y;

    float btn_x = center_x;
    float btn_y = layout_y - button_spacing;

    GameUI::DrawButton(btn_x, btn_y, button_radius, "A", GameUI::COLOUR_A);
    Engine::Print(btn_x - 20 * scale, btn_y + 25 * scale, "BRUTAL", GameUI::COLOUR_A.red, GameUI::COLOUR_A.green, GameUI::COLOUR_A.blue);

    btn_x = center_x - button_spacing;
    btn_y = layout_y;
    GameUI::DrawButton(btn_x, btn_y, button_radius, "X", GameUI::COLOUR_X);
    Engine::Print(btn_x - 15 * scale, btn_y + 25 * scale, "EASY", GameUI::COLOUR_X.red, GameUI::COLOUR_X.green, GameUI::COLOUR_X.blue);

    btn_x = center_x + button_spacing;
    btn_y = layout_y;
    GameUI::DrawButton(btn_x, btn_y, button_radius, "B", GameUI::COLOUR_B);
    Engine::Print(btn_x - 15 * scale, btn_y + 25 * scale, "HARD", GameUI::COLOUR_B.red, GameUI::COLOUR_B.green, GameUI::COLOUR_B.blue);

    btn_x = center_x;
    btn_y = layout_y + button_spacing;
    GameUI::DrawButton(btn_x, btn_y, button_radius, "Y", GameUI::COLOUR_Y);
    Engine::Print(btn_x - 20 * scale, btn_y + 25 * scale, "MEDIUM", GameUI::COLOUR_Y.red, GameUI::COLOUR_Y.green, GameUI::COLOUR_Y.blue);

    float test_x = center_x + 200 * scale;
    const float test_y = center_y;
    GameUI::DrawButton(test_x, test_y, button_radius * 0.8f, "RB", GameUI::COLOUR_GRAY);
    Engine::Print(test_x - 15 * scale, test_y + 20 * scale, "TEST", GameUI::COLOUR_GRAY.red, GameUI::COLOUR_GRAY.green, GameUI::COLOUR_GRAY.blue);

    test_x = center_x - 200 * scale;
    GameUI::DrawButton(test_x, test_y, button_radius * 0.8f, "LB", GameUI::COLOUR_GRAY);
    char buffer[64];
    (void)snprintf(buffer, sizeof(buffer), "NO DEATH MODE (%s)", no_death_mode ? "ON" : "OFF");
    Engine::Print(test_x - 40 * scale, test_y + 20 * scale, buffer, GameUI::COLOUR_GRAY.red, GameUI::COLOUR_GRAY.green, GameUI::COLOUR_GRAY.blue);

    constexpr float two_player_x = 100;
    GameUI::DrawButton(two_player_x, 150, button_radius * 0.8f, "BACK", GameUI::COLOUR_GRAY);

    (void)snprintf(buffer, sizeof(buffer), "TWO PLAYER (%s)", two_player_mode ? "ON" : "OFF");
    Engine::Print(two_player_x - 70, 200, buffer, GameUI::COLOUR_GRAY.red, GameUI::COLOUR_GRAY.green, GameUI::COLOUR_GRAY.blue);
}

void IntroScene::DrawHowToPlay(const float origin_x, const float origin_y)
{
    float y_offset = origin_y;
    const float line_height = 20;

    GameUI::PrintTextWithOutline(origin_x, y_offset, "HOW TO PLAY:", GameUI::COLOUR_WHITE);
    y_offset -= line_height;

    GameUI::PrintTextWithOutline(origin_x, y_offset, "Controller: YXAB = Up/Left/Down/Right", GameUI::COLOUR_GRAY);
    y_offset -= line_height;

    GameUI::PrintTextWithOutline(origin_x, y_offset, "Keyboard: W/A/S/D/E/R/TAB = Up/Left/Down/Right/LB/RB/Back", GameUI::COLOUR_GRAY);
    y_offset -= line_height;

    GameUI::PrintTextWithOutline(origin_x, y_offset, "Hit notes as they reach the centre!", GameUI::COLOUR_GRAY);

}
