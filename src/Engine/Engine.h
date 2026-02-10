#pragma once

#include "EngineSettings.h"
#include "Input.h"
#include "Controller.h"

#include <cstdint>

namespace Engine
{
    void DrawLine(float sx, float sy, float ex, float ey, float r = 1.0f, float g = 1.0f, float b = 1.0f);

    void DrawTriangle(float p1x, float p1y, float p1z, float p1w,
                      float p2x, float p2y, float p2z, float p2w,
                      float p3x, float p3y, float p3z, float p3w,
                      float r1, float g1, float b1,
                      float r2, float g2, float b2,
                      float r3, float g3, float b3,
                      bool is_wireframe = false);

    void Print(float x, float y, const char* text, float r = 1.0f, float g = 1.0f, float b = 1.0f, void* font = nullptr);

    void PlayAudio(const char* file_name, bool is_looping = false);
    void StopAudio(const char* file_name);
    bool IsSoundPlaying(const char* file_name);

    void LoadAudioPCM(const char* id,
                      const float* interleaved_samples,
                      uint64_t frame_count,
                      uint32_t channels,
                      uint32_t sample_rate);

    void UnloadAudio(const char* id);

    bool IsKeyPressed(Key key);

    bool WasKeyPressed(Key key);
    bool WasKeyReleased(Key key);

    const Controller& GetController(int pad = 0);

    bool RuntimeInit();
    void RuntimeShutdown();
    void RuntimePumpEvents(bool& quit);
    void RuntimeBeginFrame();
    void RuntimeEndFrame();
    void RuntimeTickInput();
    float RuntimeGetWindowWidth();
    float RuntimeGetWindowHeight();
}
