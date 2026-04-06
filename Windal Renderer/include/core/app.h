#pragma once
#include <Windows.h>
#include <iostream>

#include "core/window.h"
#include "core/scene.h"

class App
{
public:
	App() {};
	~App() {};

	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual void Update(float delta) = 0;
	virtual void Render() = 0;
	virtual void ImguiRender() = 0;

	void SetScene(Scene* scene) { mScene = scene; }

protected:
	Scene* mScene;
};