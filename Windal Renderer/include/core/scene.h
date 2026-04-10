#pragma once
#include <memory>
#include <vector>

#include "core/entities/entity.h"
#include "core/renderserver.h"
#include "graphics/camera.h"

class Scene
{
public:
    Scene() {};
    ~Scene() {};

    void Update(double delta);
    void Render(RenderServer& renderServer);
    void Shutdown();

    template<typename T, typename... Args>
    T& CreateEntity(Args&&... args)
    {
        static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");

        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

        ptr->mScene = this;

        T& reference = *ptr;
        mEntities.push_back(std::move(ptr));
        return reference;
    };

    std::vector<std::unique_ptr<Entity>>& GetEntities();

    Camera& GetCamera() { return mCamera; }

private:
    Camera mCamera;
    std::vector<std::unique_ptr<Entity>> mEntities;
};