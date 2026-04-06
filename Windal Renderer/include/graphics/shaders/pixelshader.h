#pragma once
#include <d3d11.h>
#include "graphics/shaders/shader.h"

class PixelShader : public Shader
{
public:
	PixelShader(const std::string& path);
	~PixelShader() override;

	void Bind(ID3D11DeviceContext& context) override;

private:
	ID3D11PixelShader* mPixelShader;
};