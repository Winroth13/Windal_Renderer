#include <d3d11.h>

#include "graphics/shaders/vertexshader.h"
#include "core/renderer.h"
#include "core/logger.h"

VertexShader::VertexShader(const std::string& path)
	:mVertexShader(nullptr)
{
	LoadShaderData(path, mByteCode);

	HRESULT hr = Renderer::GetDevice()->CreateVertexShader(
		mByteCode.c_str(),
		mByteCode.length(),
		nullptr,
		&mVertexShader
	);

	if (FAILED(hr))
	{
		// TODO: maybe implement some fallback shader!
		throw std::runtime_error(std::string("ERROR: Failed to create vertex shader: ") + path);
	}

	Logger::Info("Loaded vertex shader: " + path);
}

void VertexShader::Bind(RenderServer* renderServer)
{
	renderServer->GetContext()->VSSetShader(mVertexShader, nullptr, 0);
}

VertexShader::~VertexShader()
{
	mVertexShader->Release();
}