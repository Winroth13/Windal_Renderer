#pragma once
#include <memory>
#include <vector>
#include <unordered_map>

#include "core/entities/entity.h"
#include "core/renderer/renderserver.h"

#include "graphics/enviroment.h"
#include "graphics/camera.h"

class Scene
{
public:
	Scene();
	~Scene() {};

	void Begin(RenderServer& renderServer);
	void Update(double delta);
	void Render(RenderServer& renderServer);
	void Shutdown();

	template<typename T, typename... Args>
	T& CreateEntity(Args&&... args)
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");

		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

		ptr->mScene = this;

		if (mEntityNameCount.find(ptr->mName) != mEntityNameCount.end())
		{
			mEntityNameCount[ptr->mName]++;
			ptr->mName += " #" + std::to_string(mEntityNameCount[ptr->mName]);
		}
		else
		{
			mEntityNameCount[ptr->mName] = 1;
		}

		T& reference = *ptr;
		mEntities.emplace_back(std::move(ptr));
		return reference;
	};

	std::vector<std::unique_ptr<Entity>>& GetEntities();

	Camera& GetCamera() { return mCamera; }
	Enviroment& GetEnviroment() { return mEnviroment; }

	void SetLockFrustum(bool value) { mIsFrustumLocked = value; }
	bool IsFrustumLocked() { return mIsFrustumLocked; }
	bool& GetLockFrustum() { return mIsFrustumLocked; }

private:
	Camera mCamera;
	Enviroment mEnviroment;
	bool mIsFrustumLocked;

	std::unordered_map<std::string, size_t> mEntityNameCount;
	std::vector<std::unique_ptr<Entity>> mEntities;
};