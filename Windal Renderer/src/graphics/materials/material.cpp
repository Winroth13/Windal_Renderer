#include "graphics/materials/material.h"
#include "core/engine.h"
#include "core/renderer.h"
#include "core/logger.h"

Material::Material(
	std::shared_ptr<VertexShader> vertexShader,
	std::shared_ptr<PixelShader> pixelShader,
	std::shared_ptr<Texture2D> texture2D
)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = Renderer::GetDevice()->CreateInputLayout(
		inputDesc,
		3,
		vertexShader->GetByteCode().c_str(),
		vertexShader->GetByteCode().length(),
		&mInputLayout
	);

	if (FAILED(hr))
	{
		Logger::Error("Failed to create input layout");
		throw std::runtime_error("");
	}
}

Material::~Material()
{
	mInputLayout->Release();
}

void Material::Bind(RenderServer* renderServer)
{

}

void Material::SetAmbientCoefficient(const float r, const float g, const float b)
{
	mAmbientCoefficient = DirectX::XMFLOAT3(r, g, b);
}

void Material::SetDiffuseCoefficient(const float r, const float g, const float b)
{
	mDiffuseCoefficient = DirectX::XMFLOAT3(r, g, b);
}

void Material::SetSpecularCoefficient(const float r, const float g, const float b)
{
	mSpecularCoefficient = DirectX::XMFLOAT3(r, g, b);
}

void Material::SetPhongExponent(const float phongExponent)
{
	mPhongExponent = phongExponent;
}