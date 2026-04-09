#pragma once
#include "app.h"
#include "core/window.h"
#include "core/renderer.h"
#include "core/scene.h"

class Engine
{
public:
	void Run(App* app, const std::string& title, const int width, const int height);
	void Shutdown();

private:
	bool Initialize(const std::string& title, const int width, const int height);
	void Update(float delta);
	void Render();

private:
	Window mWindow;
	Renderer mRenderer;
	Scene mScene;

	App* mApp = nullptr;
};
