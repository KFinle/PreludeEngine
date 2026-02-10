#pragma once
//////////////////////////
// Abstract Scene class //
//////////////////////////////////////////////////////////////
// This game currently implements Intro, Gameplay, and End. //
//////////////////////////////////////////////////////////////

class SceneManager;

class Scene
{
protected:
    SceneManager* m_scenemanager = nullptr;
    static bool no_death_mode;
    static bool two_player_mode;

public:
    virtual ~Scene() = default;
    virtual void OnEnter(SceneManager& manager)
    {
        m_scenemanager = &manager;
    }

    virtual void OnExit(SceneManager&)
    {
        m_scenemanager = nullptr;
    }

    virtual void Update(float dt) = 0;
    virtual void Render() = 0;
};
