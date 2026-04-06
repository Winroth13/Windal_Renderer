#include "core/engine.h"

void Engine::Run(App* app, const std::string& title, const int width, const int height)
{
	mApp = app;
	mApp->SetScene(&mScene);

	if (Initialize(title, width, height))
	{
		while (mIsRunning)
		{
			Update(1);
			Render();
		}
	}
	Shutdown();
}

void Engine::Shutdown()
{
	mApp->Shutdown();
	mRenderer.Shutdown();
	mWindow.Shutdown();
}

bool Engine::Initialize(const std::string& title, const int width, const int height)
{
	if (mWindow.Create(title, width, height) == false)
	{
		std::cerr << "ERROR: Failed to create window" << std::endl;
		return false;
	}

	if (mRenderer.Create({ 0.082f, 0.769f, 0.996f, 0.0f }, &mWindow) == false)
	{
		std::cerr << "ERROR: Failed to create renderer" << std::endl;
		return false;
	}

	if (mWindow.CreateImguiWindow(&mRenderer) == false)
	{
		std::cerr << "ERROR: Failed to imgui window" << std::endl;
		return false;
	}

	mIsRunning = true;
	mApp->Initialize();

	return true;
}

void Engine::Update(float delta)
{
	if (!mWindow.PollEvents())
	{
		mIsRunning = false;
	}

	mScene.Update(delta);
	mApp->Update(delta);
}

void Engine::Render()
{
	mRenderer.BeginRender();

	mApp->Render();
	mScene.Render();

	mWindow.BeginImguiRender();
	mApp->ImguiRender();
	mWindow.EndImguiRender();

	mRenderer.EndRender();
}