#pragma once
#include <Windows.h>
#include <iostream>

#include "core/window.h"
#include "core/scene.h"
#include "core/renderserver.h"

class App
{
public:
	App() {};
	virtual ~App() {};

	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual void Update(float delta) = 0;
	virtual void Render(RenderServer& renderServer) = 0;
	virtual void ImguiRender() = 0;

	void SetScene(Scene* scene) { mScene = scene; }

protected:
	Scene* mScene;
};