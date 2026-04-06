#pragma once
#include <iostream>
#include <Windows.h>
#include <string>

#include "console/console.h"
#include "core/app.h"
#include "core/engine.h"

// Memory leak debugging
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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
	// Memory leak debugging on program exit
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

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