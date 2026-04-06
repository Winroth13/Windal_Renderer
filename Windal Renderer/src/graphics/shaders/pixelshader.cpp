#include "graphics/shaders/pixelshader.h"
#include "core/renderer.h"
#include "core/logger.h"

PixelShader::PixelShader(const std::string& path)
	:mPixelShader(nullptr)
{
	std::string shaderByteCode;
	LoadShaderData(path, shaderByteCode);

	HRESULT hr = Renderer::GetDevice()->CreatePixelShader(
		shaderByteCode.c_str(),
		shaderByteCode.length(),
		nullptr,
		&mPixelShader
	);

	if (FAILED(hr))
	{
		// TODO: maybe implement some fallback shader!
		throw std::runtime_error(std::string("ERROR: Failed to create pixel shader: ") + path);
	}

	Logger::Info("Loaded pixel shader: " + path);
}

void PixelShader::Bind(RenderServer* renderServer)
{
	renderServer->GetContext()->PSSetShader(mPixelShader, nullptr, 0);
}

PixelShader::~PixelShader()
{
	mPixelShader->Release();
}