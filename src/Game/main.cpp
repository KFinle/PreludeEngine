#if BUILD_PLATFORM_WINDOWS
#include <windows.h>
#endif

#include "Scenes/SceneManager.h"
static SceneManager scenemanager;
#include "Debug/DebugLogger.h"

//////////
// Init //
//////////
void Init()
{
    Logger::PrintLog(Logger::GAME, "Initializing Game");
    scenemanager.Launch(SceneType::Intro);
}

////////////
// Update //
////////////
void Update(const float delta_time)
{
    const float dt_sec = delta_time * 0.001f;
    scenemanager.Update(dt_sec);
}

////////////
// Render //
////////////
void Render()
{
    scenemanager.Render();
}

//////////////
// Shutdown //
//////////////
void Shutdown()
{
    Logger::PrintLog(Logger::GAME, "Shutdown");
    scenemanager.Clear();
}
