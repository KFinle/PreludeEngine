#pragma once
#include "Scene.h"

class IntroScene : public Scene
{
public:
    IntroScene();
    void Update(float dt) override;
    void Render() override;

private:
    float m_intro_countdown = 3.0f;
    inline static bool m_sound_warning_triggered = false;
    void DrawMenu(float center_x, float center_y, float scale);
    void DrawHowToPlay(float position_x, float position_y);
};
