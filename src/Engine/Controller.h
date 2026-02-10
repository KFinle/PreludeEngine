#pragma once

#include <cstdint>
#include "Input.h"

namespace Engine
{
    class Controller
    {
    public:
        bool CheckButton(GamepadButton button, bool on_press = true) const;

        float GetLeftThumbStickX() const;
        float GetLeftThumbStickY() const;
        float GetRightThumbStickX() const;
        float GetRightThumbStickY() const;
        float GetLeftTrigger() const;
        float GetRightTrigger() const;

        void SetConnected(bool connected);
        void SetButtons(uint32_t buttons);
        void SetAxes(int16_t lx, int16_t ly, int16_t rx, int16_t ry, int16_t lt, int16_t rt);
        void AdvanceFrame();

    private:
        bool m_connected = false;

        uint32_t m_buttons = 0;
        uint32_t m_last_buttons = 0;

        int16_t m_left_x = 0;
        int16_t m_left_y = 0;
        int16_t m_right_x = 0;
        int16_t m_right_y = 0;
        int16_t m_left_trigger = 0;
        int16_t m_right_trigger = 0;
    };
}
