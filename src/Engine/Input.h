#pragma once

namespace Engine
{
    enum Key
    {
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,

        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_0,

        KEY_SPACE,
        KEY_ESC,
        KEY_ENTER,
        KEY_TAB,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_UP,
        KEY_DOWN,
        KEY_HOME,
        KEY_END,
        KEY_INSERT,

        KEY_COUNT,
    };

    enum GamepadButton
    {
        BTN_A = 0x0001,
        BTN_B = 0x0002,
        BTN_X = 0x0004,
        BTN_Y = 0x0008,

        BTN_START = 0x0010,
        BTN_BACK = 0x0020,

        BTN_LBUMPER = 0x0040,
        BTN_LSTICK = 0x0080,
        BTN_RBUMPER = 0x0100,
        BTN_RSTICK = 0x0200,

        BTN_DPAD_LEFT = 0x0400,
        BTN_DPAD_RIGHT = 0x0800,
        BTN_DPAD_UP = 0x1000,
        BTN_DPAD_DOWN = 0x2000,
    };
}
