#include "SceneManager.h"
#include "Scene.h"

#include "IntroScene.h"
#include "GameplayScene.h"
#include "EndScene.h"

// toggle for timing punishments
bool Scene::no_death_mode = false;
bool Scene::two_player_mode = false;

SceneManager::SceneManager() = default;
SceneManager::~SceneManager() = default;

void SceneManager::Launch(const SceneType type, const GameMode mode)
{
    m_has_pending = false;
    m_current_mode = mode;

    switch (type)
    {
        case SceneType::Intro:
            SetScene(std::make_unique<IntroScene>());
            break;

        case SceneType::Gameplay:
            SetScene(std::make_unique<GameplayScene>());
            break;

        case SceneType::End:
            SetScene(std::make_unique<EndScene>());
            break;
    }
}

void SceneManager::Request(const SceneType type, const GameMode mode)
{
    m_has_pending = true;
    m_pending_type = type;
    m_pending_mode = mode;
}

void SceneManager::Clear()
{
    m_has_pending = false;
    if (m_scene) m_scene->OnExit(*this);
    m_scene.reset();
}

void SceneManager::SetScene(std::unique_ptr<Scene> next)
{
    if (m_scene) m_scene->OnExit(*this);
    m_scene = std::move(next);
    if (m_scene) m_scene->OnEnter(*this);
}

void SceneManager::Update(const float dt)
{
    if (m_scene) m_scene->Update(dt);
    if (m_has_pending)
    {
        m_has_pending = false;
        Launch(m_pending_type, m_pending_mode);
    }
}

void SceneManager::Render()
{
    if (m_scene) m_scene->Render();
}
