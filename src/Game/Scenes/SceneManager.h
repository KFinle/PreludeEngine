#pragma once
#include <memory>

///////////////////
// Scene Manager //
///////////////////////////////////////////////////////////////////////////////////////
// Handles the routing of Scenes. Scenes must request redirect from the SceneManager //
///////////////////////////////////////////////////////////////////////////////////////

class Scene;
enum class SceneType
{
    Intro,
    Gameplay,
    End
};

enum class GameMode
{
    Easy,
    Medium,
    Hard,
    Brutal,
    Test
};

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();
    GameMode GetCurrentMode() const { return m_current_mode; }
    void Launch(SceneType type, GameMode mode = GameMode::Easy);
    void Request(SceneType type, GameMode mode = GameMode::Easy);
    void Clear();
    void Update(float dt);
    void Render();

private:
    std::unique_ptr<Scene> m_scene;
    bool m_has_pending = false;
    SceneType m_pending_type = SceneType::Intro;
    GameMode m_current_mode = GameMode::Easy;
    GameMode m_pending_mode = GameMode::Easy;

    void SetScene(std::unique_ptr<Scene> scene);
};
