#include "graphics/materials/material.h"
#include "core/renderer.h"
#include "core/logger.h"

#include <iostream>

#include "core/imguiflags.h"
#include "imgui/imgui.h"
#define TEXTURE_SIZE 256

Material::Material(
	std::shared_ptr<VertexShader> vertexShader,
	std::shared_ptr<PixelShader> pixelShader,
	std::shared_ptr<Texture2D> texture2D
) : mVertexShader(vertexShader), mPixelShader(pixelShader), mTexture(texture2D)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
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

	PerMaterial perMaterialBuffer = {};
	D3D11_BUFFER_DESC perMaterialBufferDesc = {};
	perMaterialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	perMaterialBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	perMaterialBufferDesc.CPUAccessFlags = 0;
	perMaterialBufferDesc.MiscFlags = 0;
	perMaterialBufferDesc.ByteWidth = sizeof(perMaterialBuffer);
	perMaterialBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &perMaterialBuffer;

	hr = Renderer::GetDevice()->CreateBuffer(
		&perMaterialBufferDesc,
		&data,
		&mBuffer
	);

	if (FAILED(hr))
	{
		Logger::Error("Failed to create material constant buffer");
		throw std::runtime_error("");
	}
}

Material::~Material()
{
	mInputLayout->Release();
	mBuffer->Release();
	Logger::Info("Material destructor!");
}

void Material::Bind(RenderServer& renderServer)
{
	if (mIsDirty)
	{
		UpdateBuffer(renderServer);
		mIsDirty = false;
	}

	renderServer.GetContext()->IASetInputLayout(mInputLayout);
	renderServer.GetContext()->PSSetConstantBuffers(
		BUFFER_PER_MATERIAL,
		1,
		&mBuffer
	);

	mVertexShader->Bind(renderServer);
	mPixelShader->Bind(renderServer);
	mTexture->Bind(renderServer, 0); // Albedo Texture
}

void Material::Unbind(RenderServer& renderServer)
{
	mVertexShader->Unbind(renderServer);
	mPixelShader->Unbind(renderServer);
	// TODO: Unbind textures?
}

void Material::SetAmbientCoefficient(const float r, const float g, const float b)
{
	mAmbientCoefficient = DirectX::XMFLOAT3(r, g, b);
	mIsDirty = true;
}

void Material::SetDiffuseCoefficient(const float r, const float g, const float b)
{
	mDiffuseCoefficient = DirectX::XMFLOAT3(r, g, b);
	mIsDirty = true;
}

void Material::SetSpecularCoefficient(const float r, const float g, const float b)
{
	mSpecularCoefficient = DirectX::XMFLOAT3(r, g, b);
	mIsDirty = true;
}

void Material::SetPhongExponent(const float phongExponent)
{
	mPhongExponent = phongExponent;
	mIsDirty = true;
}

void Material::UpdateBuffer(RenderServer& renderServer)
{
	PerMaterial buffer = {};
	buffer.phongExponent = mPhongExponent;

	buffer.ambientCoefficient = {
		mAmbientCoefficient.x,
		mAmbientCoefficient.y ,
		mAmbientCoefficient.z
	};

	buffer.diffuseCoefficient = {
		mDiffuseCoefficient.x,
		mDiffuseCoefficient.y ,
		mDiffuseCoefficient.z
	};

	buffer.specularCoefficient = {
		mSpecularCoefficient.x,
		mSpecularCoefficient.y ,
		mSpecularCoefficient.z
	};

	renderServer.GetContext()->UpdateSubresource(
		mBuffer, 0, NULL, &buffer, 0, 0
	);
}

void Material::RenderImgui()
{
	if (ImGui::TreeNodeEx("Shaders", TREE_NODE_FLAGS))
	{
		ImGui::Text("Vertex Shader: %s", mVertexShader->GetPath().c_str());
		ImGui::Text("Pixel Shader: %s", mPixelShader->GetPath().c_str());
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Light Parameters", TREE_NODE_FLAGS))
	{
		if (ImGui::DragFloat3("Ambient", &mAmbientCoefficient.x, 0.01f, 0.0))
		{
			mIsDirty = true;
		}
		if (ImGui::DragFloat3("Diffuse", &mDiffuseCoefficient.x, 0.01f, 0.0))
		{
			mIsDirty = true;
		}
		if (ImGui::DragFloat3("Specular", &mSpecularCoefficient.x, 0.01f, 0.0))
		{
			mIsDirty = true;
		}

		if (ImGui::DragFloat("Phong Exponent", &mPhongExponent, 1, 0.0))
		{
			mIsDirty = true;
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Diffuse Texture", TREE_NODE_FLAGS))
	{
		mTexture->RenderImgui(TEXTURE_SIZE, TEXTURE_SIZE);
		ImGui::TreePop();
	}
}