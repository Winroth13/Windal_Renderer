#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <d3d11.h>

class Shader
{
public:
	Shader() = default;
	virtual ~Shader() = default;

	virtual void Bind(ID3D11DeviceContext& context) {};

protected:
	bool LoadShaderData(const std::string& path, std::string& data);
};