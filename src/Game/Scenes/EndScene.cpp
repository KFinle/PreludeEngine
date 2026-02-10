#include "EndScene.h"

#include <cstdio>

#include "Engine/Engine.h"
#include "Gameplay/RunResults.h"
#include "Scenes/SceneManager.h"

EndScene::EndScene() = default;

void EndScene::Update(float)
{
    if (!m_scenemanager) return;

    if (Engine::GetController().CheckButton(Engine::BTN_A, true) || Engine::IsKeyPressed(Engine::KEY_ENTER))
    {
        m_scenemanager->Request(SceneType::Intro);
    }
}

void EndScene::Render()
{

    // TODO:
    // come back and make this screen nicer
    
    const float header_gap = 30;
    const float stat_gap = 20;
    const float stat_gap_wide = 25;
    const float section_gap = 40;
    float cursor_y = APP_VIRTUAL_HEIGHT - 80;

    auto print_end_text = [](const float y_pos, const char* text)
    {
        constexpr float char_width = 12.0f;
        int text_length = 0;
        while (text[text_length] != '\0') ++text_length;
        const float text_width = static_cast<float>(text_length) * char_width;
        const float center_x = APP_VIRTUAL_WIDTH * 0.5f;
        const float text_x = center_x - text_width * 0.5f;
        Engine::Print(text_x, y_pos, text);
    };

    struct ResultSummary
    {
        int total_notes = 0;
        int total_hits = 0;
        float accuracy = 0.0f;
        float hit_rate = 0.0f;
        const char* grade = "D";
    };

    auto build_summary = [](const RunResults& results)
    {
        ResultSummary summary{};
        summary.total_notes = results.perfect + results.good + results.late + results.missed;
        summary.total_hits = summary.total_notes - results.missed;

        if (summary.total_notes > 0)
        {
            const float weighted = static_cast<float>(results.perfect) * 1.00f +
                                   static_cast<float>(results.good) * 0.70f +
                                   static_cast<float>(results.late) * 0.40f;
            
            summary.accuracy = (weighted / static_cast<float>(summary.total_notes)) * 100.0f;
            summary.hit_rate = static_cast<float>(summary.total_hits) / static_cast<float>(summary.total_notes) * 100.0f;
        }

        if (summary.accuracy >= 95.0f) summary.grade = "S";
        else if (summary.accuracy >= 90.0f) summary.grade = "A";
        else if (summary.accuracy >= 80.0f) summary.grade = "B";
        else if (summary.accuracy >= 70.0f) summary.grade = "C";
        else summary.grade = "D";
        return summary;
    };

    auto print_results_block = [&](const float start_y, const char* header, const RunResults& results)
    {
        float current_y = start_y;
        const ResultSummary summary = build_summary(results);

        print_end_text(current_y, header);
        current_y -= header_gap;

        char text_buffer[96];
        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Perfect %4d", results.perfect);
        print_end_text(current_y, text_buffer);
        current_y -= stat_gap;

        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Good %4d", results.good);
        print_end_text(current_y, text_buffer);
        current_y -= stat_gap;

        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Late %4d", results.late);
        print_end_text(current_y, text_buffer);
        current_y -= stat_gap;

        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Missed %4d", results.missed);
        print_end_text(current_y, text_buffer);
        current_y -= stat_gap_wide;

        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Total Notes %4d", summary.total_notes);
        print_end_text(current_y, text_buffer);
        current_y -= stat_gap;

        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Hit Rate %3.1f%%", summary.hit_rate);
        print_end_text(current_y, text_buffer);
        current_y -= stat_gap;

        (void)std::snprintf(text_buffer, sizeof(text_buffer), "Accuracy %3.1f%%  Grade %s", summary.accuracy, summary.grade);
        print_end_text(current_y, text_buffer);
        current_y -= section_gap;

        return current_y;
    };

    print_end_text(cursor_y, "GAME OVER");
    cursor_y -= header_gap;

    if (two_player_mode)
    {
        print_end_text(cursor_y, "TWO PLAYER RESULTS");
        cursor_y -= header_gap;
        cursor_y = print_results_block(cursor_y, "LEFT PLAYER", run_results);
        cursor_y = print_results_block(cursor_y, "RIGHT PLAYER", run_results_p2);
    }
    else
    {
        print_end_text(cursor_y, "RESULTS");
        cursor_y -= header_gap;
        cursor_y = print_results_block(cursor_y, "TOTAL", run_results);
    }
    print_end_text(cursor_y, "Press A/[Enter] to return to title screen");
}
