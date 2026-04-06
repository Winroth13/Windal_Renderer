#include "core/engine.h"
#include "core/logger.h"

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
	mScene.Shutdown();
	mApp->Shutdown();
	mRenderer.Shutdown();
	mWindow.Shutdown();
}

bool Engine::Initialize(const std::string& title, const int width, const int height)
{
	if (mWindow.Create(title, width, height) == false)
	{
		Logger::Error("Failed to create window");
		return false;
	}

	if (mRenderer.Create({ 0.082f, 0.769f, 0.996f, 0.0f }, &mWindow) == false)
	{
		Logger::Error("Failed to create renderer");
		return false;
	}

	if (mWindow.CreateImguiWindow(&mRenderer) == false)
	{
		Logger::Error("Failed to imgui window");
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

	mApp->Render(mRenderer.GetRenderServer());
	mScene.Render(mRenderer.GetRenderServer());

	mWindow.BeginImguiRender();
	mApp->ImguiRender();
	mWindow.EndImguiRender();

	mRenderer.EndRender();
}