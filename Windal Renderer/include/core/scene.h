#pragma once
#include <memory>
#include <vector>
#include "core/entity.h"

class Scene
{
public:
    Scene() {};
    ~Scene() {};

    void Update();
    void Render();
    void Shutdown();

private:
    std::vector<std::unique_ptr<Entity>> mEntities;
};