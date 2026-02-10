#pragma once
#include "Scene.h"

class EndScene : public Scene
{
public:
    EndScene();
    void Update(float dt) override;
    void Render() override;
};
