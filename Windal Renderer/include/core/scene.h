#pragma once
#include <memory>
#include <vector>
#include "core/entity.h"

class Scene
{
public:
    Scene() {};
    ~Scene() {};

    void Update(float delta);
    void Render();
    void Shutdown();

    template<typename T, typename... Args>
    T& CreateEntity(Args&&... args)
    {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T& reference = *ptr;
        mEntities.push_back(std::move(ptr));
        return reference;
    };

    std::vector<std::unique_ptr<Entity>>& GetEntities();

private:
    std::vector<std::unique_ptr<Entity>> mEntities;
};