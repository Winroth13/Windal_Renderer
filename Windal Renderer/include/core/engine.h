#pragma once
#include "app.h"
#include "core/window.h"
#include "core/renderer.h"

class Engine
{
public:
	void Run(App* app, const std::string& title, const int width, const int height);
	void Shutdown();

private:
	bool Initialize(const std::string& title, const int width, const int height);
	void Update();
	void Render();

private:
	bool mIsRunning = false;

	Window mWindow;
	Renderer mRenderer;

	App* mApp;
};
