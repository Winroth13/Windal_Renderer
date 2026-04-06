#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <d3d11.h>

#include "core/renderserver.h"

class Shader
{
public:
	Shader() = default;
	virtual ~Shader() = default;

	virtual void Bind(RenderServer* renderServer) {};

protected:
	bool LoadShaderData(const std::string& path, std::string& data);
};