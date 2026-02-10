////////////////////////////////////////////
// TODO: Return to this file and refactor //
///////////////////////////////////////////////////////////////////////////////////////////////
// This file is a quick and dirty implementation to migrate from the previous restricted API //
///////////////////////////////////////////////////////////////////////////////////////////////
#include "Engine.h"
#include <algorithm>
#include <SDL3/SDL.h>
#include "AudioPlayer.h"

namespace Engine
{
    static SDL_Window* window = nullptr;
    static SDL_Renderer* renderer = nullptr;

    static constexpr int max_controllers = 4;
    static Controller g_controllers[max_controllers];

    static constexpr int key_count = static_cast<int>(KEY_COUNT);
    static uint8_t current_key[key_count] = {};
    static uint8_t previous_key[key_count] = {};

    static float ToRenderX(const float x)
    {
        return x;
    }

    static float ToRenderY(const float y)
    {
        const float h = RuntimeGetWindowHeight();
        return h - y;
    }

    static void SetDrawColour(const float r, const float g, const float b, const float a)
    {
        (void)SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);
    }

    bool RuntimeInit()
    {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
        {
            return false;
        }

        window = SDL_CreateWindow(APP_WINDOW_TITLE, APP_INIT_WINDOW_WIDTH, APP_INIT_WINDOW_HEIGHT, 0);
        if (!window) return false;

        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) return false;

        (void)SDL_SetRenderLogicalPresentation(renderer, APP_VIRTUAL_WIDTH, APP_VIRTUAL_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

        if (!AudioPlayer::Get().Initialize())
        {
            return false;
        }

        RuntimeTickInput();

        return true;
    }

    void RuntimeShutdown()
    {
        AudioPlayer::Get().Shutdown();

        if (renderer)
        {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }

        if (window)
        {
            SDL_DestroyWindow(window);
            window = nullptr;
        }

        SDL_Quit();
    }

    void RuntimePumpEvents(bool& quit)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT)
            {
                quit = true;
            }
        }
    }

    void RuntimeBeginFrame()
    {
        SetDrawColour(0.0f, 0.0f, 0.0f, 1.0f);
        (void)SDL_RenderClear(renderer);
    }

    void RuntimeEndFrame()
    {
        SDL_RenderPresent(renderer);
    }

    float RuntimeGetWindowWidth()
    {
        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window, &w, &h);
        return static_cast<float>(w);
    }

    float RuntimeGetWindowHeight()
    {
        int w = 0;
        int h = 0;
        SDL_GetWindowSize(window, &w, &h);
        return static_cast<float>(h);
    }

    static SDL_Scancode ScancodeForKey(const Key key)
    {
        switch (key)
        {
            case KEY_A: return SDL_SCANCODE_A;
            case KEY_B: return SDL_SCANCODE_B;
            case KEY_C: return SDL_SCANCODE_C;
            case KEY_D: return SDL_SCANCODE_D;
            case KEY_E: return SDL_SCANCODE_E;
            case KEY_F: return SDL_SCANCODE_F;
            case KEY_G: return SDL_SCANCODE_G;
            case KEY_H: return SDL_SCANCODE_H;
            case KEY_I: return SDL_SCANCODE_I;
            case KEY_J: return SDL_SCANCODE_J;
            case KEY_K: return SDL_SCANCODE_K;
            case KEY_L: return SDL_SCANCODE_L;
            case KEY_M: return SDL_SCANCODE_M;
            case KEY_N: return SDL_SCANCODE_N;
            case KEY_O: return SDL_SCANCODE_O;
            case KEY_P: return SDL_SCANCODE_P;
            case KEY_Q: return SDL_SCANCODE_Q;
            case KEY_R: return SDL_SCANCODE_R;
            case KEY_S: return SDL_SCANCODE_S;
            case KEY_T: return SDL_SCANCODE_T;
            case KEY_U: return SDL_SCANCODE_U;
            case KEY_V: return SDL_SCANCODE_V;
            case KEY_W: return SDL_SCANCODE_W;
            case KEY_X: return SDL_SCANCODE_X;
            case KEY_Y: return SDL_SCANCODE_Y;
            case KEY_Z: return SDL_SCANCODE_Z;

            case KEY_1: return SDL_SCANCODE_1;
            case KEY_2: return SDL_SCANCODE_2;
            case KEY_3: return SDL_SCANCODE_3;
            case KEY_4: return SDL_SCANCODE_4;
            case KEY_5: return SDL_SCANCODE_5;
            case KEY_6: return SDL_SCANCODE_6;
            case KEY_7: return SDL_SCANCODE_7;
            case KEY_8: return SDL_SCANCODE_8;
            case KEY_9: return SDL_SCANCODE_9;
            case KEY_0: return SDL_SCANCODE_0;

            case KEY_SPACE: return SDL_SCANCODE_SPACE;
            case KEY_ESC: return SDL_SCANCODE_ESCAPE;
            case KEY_ENTER: return SDL_SCANCODE_RETURN;
            case KEY_TAB: return SDL_SCANCODE_TAB;
            case KEY_LEFT: return SDL_SCANCODE_LEFT;
            case KEY_RIGHT: return SDL_SCANCODE_RIGHT;
            case KEY_UP: return SDL_SCANCODE_UP;
            case KEY_DOWN: return SDL_SCANCODE_DOWN;
            case KEY_HOME: return SDL_SCANCODE_HOME;
            case KEY_END: return SDL_SCANCODE_END;
            case KEY_INSERT: return SDL_SCANCODE_INSERT;
            default: return SDL_SCANCODE_UNKNOWN;
        }
    }

    bool IsKeyPressed(const Key key)
    {
        if (key < 0 || key >= key_count) return false;
        return current_key[key] != 0;
    }

    bool WasKeyPressed(const Key key)
    {
        if (key < 0 || key >= key_count) return false;
        return current_key[key] != 0 && previous_key[key] == 0;
    }

    bool WasKeyReleased(const Key key)
    {
        if (key < 0 || key >= key_count) return false;
        return current_key[key] == 0 && previous_key[key] != 0;
    }

    bool Controller::CheckButton(const GamepadButton button, const bool on_press) const
    {
        const uint32_t mask = static_cast<uint32_t>(button);
        if (on_press)
        {
            return (m_buttons & mask) && !(m_last_buttons & mask);
        }
        return (m_buttons & mask) != 0;
    }

    static constexpr float kThumbMax = 32768.0f;
    static constexpr float kTriggerMax = 32768.0f;

    float Controller::GetLeftThumbStickX() const { return static_cast<float>(m_left_x) / kThumbMax; }
    float Controller::GetLeftThumbStickY() const { return static_cast<float>(m_left_y) / kThumbMax; }
    float Controller::GetRightThumbStickX() const { return static_cast<float>(m_right_x) / kThumbMax; }
    float Controller::GetRightThumbStickY() const { return static_cast<float>(m_right_y) / kThumbMax; }
    float Controller::GetLeftTrigger() const { return static_cast<float>(m_left_trigger) / kTriggerMax; }
    float Controller::GetRightTrigger() const { return static_cast<float>(m_right_trigger) / kTriggerMax; }

    void Controller::SetConnected(const bool connected) { m_connected = connected; }
    void Controller::SetButtons(const uint32_t buttons) { m_buttons = buttons; }
    void Controller::SetAxes(const int16_t lx, const int16_t ly, const int16_t rx, const int16_t ry, const int16_t lt, const int16_t rt)
    {
        m_left_x = lx;
        m_left_y = ly;
        m_right_x = rx;
        m_right_y = ry;
        m_left_trigger = lt;
        m_right_trigger = rt;
    }

    void Controller::AdvanceFrame()
    {
        m_last_buttons = m_buttons;
    }

    static uint32_t MapButtons(SDL_Gamepad* pad)
    {
        if (!pad) return 0;

        uint32_t buttons = 0;

        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_SOUTH)) buttons |= BTN_A;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_EAST)) buttons |= BTN_B;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_WEST)) buttons |= BTN_X;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_NORTH)) buttons |= BTN_Y;

        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_START)) buttons |= BTN_START;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_BACK)) buttons |= BTN_BACK;

        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)) buttons |= BTN_DPAD_LEFT;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) buttons |= BTN_DPAD_RIGHT;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_UP)) buttons |= BTN_DPAD_UP;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) buttons |= BTN_DPAD_DOWN;

        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER)) buttons |= BTN_LBUMPER;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)) buttons |= BTN_RBUMPER;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_LEFT_STICK)) buttons |= BTN_LSTICK;
        if (SDL_GetGamepadButton(pad, SDL_GAMEPAD_BUTTON_RIGHT_STICK)) buttons |= BTN_RSTICK;

        return buttons;
    }

    void RuntimeTickInput()
    {
        for (int i = 0; i < key_count; i++)
        {
            previous_key[i] = current_key[i];
        }

        const bool* keyboard = SDL_GetKeyboardState(nullptr);
        for (int i = 0; i < key_count; i++)
        {
            const SDL_Scancode scan = ScancodeForKey(static_cast<Key>(i));
            current_key[i] = (scan != SDL_SCANCODE_UNKNOWN && keyboard[scan]) ? static_cast<uint8_t>(1) : static_cast<uint8_t>(0);
        }

        SDL_UpdateGamepads();

        for (int i = 0; i < max_controllers; i++)
        {
            g_controllers[i].AdvanceFrame();
            g_controllers[i].SetConnected(false);
            g_controllers[i].SetButtons(0);
            g_controllers[i].SetAxes(0, 0, 0, 0, 0, 0);
        }

        int count = 0;
        SDL_JoystickID* pads = SDL_GetGamepads(&count);
        if (!pads)
        {
            return;
        }

        count = std::min(count, max_controllers);
        for (int i = 0; i < count; i++)
        {
            SDL_Gamepad* pad = SDL_OpenGamepad(pads[i]);
            if (!pad) continue;

            g_controllers[i].SetConnected(true);
            g_controllers[i].SetButtons(MapButtons(pad));

            const int16_t lx = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTX);
            const int16_t ly = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFTY);
            const int16_t rx = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTX);
            const int16_t ry = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHTY);
            const int16_t lt = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
            const int16_t rt = SDL_GetGamepadAxis(pad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);
            g_controllers[i].SetAxes(lx, ly, rx, ry, lt, rt);

            SDL_CloseGamepad(pad);
        }

        SDL_free(pads);
    }

    const Controller& GetController(const int pad)
    {
        const int idx = (pad < 0 || pad >= max_controllers) ? 0 : pad;
        return g_controllers[idx];
    }

    void DrawLine(const float sx, const float sy, const float ex, const float ey, const float r, const float g, const float b)
    {
        SetDrawColour(r, g, b, 1.0f);
        (void)SDL_RenderLine(renderer, ToRenderX(sx), ToRenderY(sy), ToRenderX(ex), ToRenderY(ey));
    }

    void DrawTriangle(const float p1x, const float p1y, const float p1z, const float p1w,
                      const float p2x, const float p2y, const float p2z, const float p2w,
                      const float p3x, const float p3y, const float p3z, const float p3w,
                      const float r1, const float g1, const float b1,
                      const float r2, const float g2, const float b2,
                      const float r3, const float g3, const float b3,
                      const bool is_wireframe)
    {
        (void)p1z;
        (void)p1w;
        (void)p2z;
        (void)p2w;
        (void)p3z;
        (void)p3w;

        if (is_wireframe)
        {
            DrawLine(p1x, p1y, p2x, p2y, r1, g1, b1);
            DrawLine(p2x, p2y, p3x, p3y, r2, g2, b2);
            DrawLine(p3x, p3y, p1x, p1y, r3, g3, b3);
            return;
        }

        SDL_Vertex verts[3];
        verts[0].position.x = ToRenderX(p1x);
        verts[0].position.y = ToRenderY(p1y);
        verts[0].color.r = r1;
        verts[0].color.g = g1;
        verts[0].color.b = b1;
        verts[0].color.a = 1.0f;
        verts[0].tex_coord.x = 0.0f;
        verts[0].tex_coord.y = 0.0f;

        verts[1].position.x = ToRenderX(p2x);
        verts[1].position.y = ToRenderY(p2y);
        verts[1].color.r = r2;
        verts[1].color.g = g2;
        verts[1].color.b = b2;
        verts[1].color.a = 1.0f;
        verts[1].tex_coord.x = 0.0f;
        verts[1].tex_coord.y = 0.0f;

        verts[2].position.x = ToRenderX(p3x);
        verts[2].position.y = ToRenderY(p3y);
        verts[2].color.r = r3;
        verts[2].color.g = g3;
        verts[2].color.b = b3;
        verts[2].color.a = 1.0f;
        verts[2].tex_coord.x = 0.0f;
        verts[2].tex_coord.y = 0.0f;

        (void)SDL_RenderGeometry(renderer, nullptr, verts, 3, nullptr, 0);
    }

    void Print(const float x, const float y, const char* text, const float r, const float g, const float b, void* font)
    {
        (void)font;
        SetDrawColour(r, g, b, 1.0f);
        (void)SDL_RenderDebugText(renderer, ToRenderX(x), ToRenderY(y), text);
    }

    void PlayAudio(const char* file_name, const bool is_looping)
    {
        const SoundFlags flags = is_looping ? SoundFlags::Looping : SoundFlags::None;
        (void)AudioPlayer::Get().Play(file_name, flags);
    }

    void StopAudio(const char* file_name)
    {
        (void)AudioPlayer::Get().Stop(file_name);
    }

    bool IsSoundPlaying(const char* file_name)
    {
        return AudioPlayer::Get().IsPlaying(file_name);
    }

    void LoadAudioPCM(const char* id,
                      const float* interleaved_samples,
                      const uint64_t frame_count,
                      const uint32_t channels,
                      const uint32_t sample_rate)
    {
        (void)AudioPlayer::Get().PlayPcmF32(id, interleaved_samples, frame_count, channels, sample_rate, SoundFlags::None);
    }

    void UnloadAudio(const char* id)
    {
        (void)AudioPlayer::Get().Unload(id);
    }
}
