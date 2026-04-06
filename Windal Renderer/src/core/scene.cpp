#include "core/scene.h"

void Scene::Update(float delta)
{
	for (std::unique_ptr<Entity>& e : mEntities)
	{
		e->Update(delta);
	}
}

void Scene::Render()
{
	for (std::unique_ptr<Entity>& e : mEntities)
	{
		e->Render();
	}
}

void Scene::Shutdown()
{
	mEntities.clear();
}

std::vector<std::unique_ptr<Entity>>& Scene::GetEntities()
{
	return mEntities;
}
