#include "Engine.h"

#include <SDL3/SDL.h>

extern void Init();
extern void Update(float delta_time_ms);
extern void Render();
extern void Shutdown();

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    if (!Engine::RuntimeInit())
    {
        return 1;
    }

    Init();

    bool quit = false;
    uint64_t last_ticks = SDL_GetTicksNS();
    const uint64_t target_frame_ns = static_cast<uint64_t>((1.0 / static_cast<double>(APP_MAX_FRAME_RATE)) * 1000.0 * 1000.0 * 1000.0);

    while (!quit)
    {
        const uint64_t now_ticks = SDL_GetTicksNS();
        const uint64_t delta_ns = now_ticks - last_ticks;
        const float delta_ms = static_cast<float>(static_cast<double>(delta_ns) / 1000000.0);

        if (delta_ns < target_frame_ns)
        {
            SDL_DelayNS(target_frame_ns - delta_ns);
            continue;
        }

        last_ticks = now_ticks;

        Engine::RuntimePumpEvents(quit);
        Engine::RuntimeTickInput();

        Update(delta_ms);

        Engine::RuntimeBeginFrame();
        Render();
        Engine::RuntimeEndFrame();
    }

    Shutdown();
    Engine::RuntimeShutdown();
    return 0;
}
