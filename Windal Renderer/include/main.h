#pragma once
#include <iostream>
#include <Windows.h>
#include <string>

#include "console/console.h"
#include "core/app.h"
#include "core/engine.h"

struct WindowProps
{
	std::string title;
	int width;
	int height;
};

App* CreateApp();
WindowProps CreateWindowProperties();

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow
)
{
	CreateNewConsole(1024);

	App* app = CreateApp();
	WindowProps props = CreateWindowProperties();

	Engine* engine = new Engine();
	engine->Run(app, props.title, props.width, props.height);

	delete engine;
	delete app;

	ReleaseConsole();

	return 0;
}