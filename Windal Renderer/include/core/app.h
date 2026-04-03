#pragma once
#include <Windows.h>
#include <iostream>

#include "core/window.h"

class App
{
public:
	App() {};
	~App() {};

	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void ImguiRender() = 0;
};