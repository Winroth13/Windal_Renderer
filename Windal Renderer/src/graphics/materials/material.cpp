#include "graphics/materials/material.h"
#include "core/renderer.h"
#include "core/logger.h"

#include <iostream>

#include "core/imguiflags.h"
#include "imgui/imgui.h"
#define TEXTURE_SIZE 256

Material::Material(
	std::shared_ptr<VertexShader> vertexShader,
	std::shared_ptr<Texture2D> texture2D
) : mVertexShader(vertexShader), mTexture(texture2D)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc[4] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = Renderer::GetDevice()->CreateInputLayout(
		inputDesc,
		4,
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
	Logger::Info("Material destructor!");
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

void Material::SetReflectiveness(const float reflectiveness)
{
	mReflectiveness = reflectiveness;
	mIsDirty = true;
}

void Material::SetNormalMap(std::shared_ptr<Texture2D> normalMap)
{
	mNormalMap = normalMap;
	if (mNormalMap)
	{
		mFlags |= static_cast<uint32_t>(MaterialFlags::HAS_NORMAL_MAP);
	}
	else
	{
		mFlags &= ~static_cast<uint32_t>(MaterialFlags::HAS_NORMAL_MAP);
	}
}

void Material::RenderImgui()
{
	if (ImGui::TreeNodeEx("Shaders", TREE_NODE_FLAGS))
	{
		ImGui::Text("Vertex Shader: %s", mVertexShader->GetPath().c_str());
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

		if (ImGui::DragFloat("Reflectiveness", &mReflectiveness, 0.02f, 0.0, 1.0f))
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

	if (ImGui::TreeNodeEx("Normal Texture", TREE_NODE_FLAGS))
	{
		if (mNormalMap != nullptr)
		{
			mNormalMap->RenderImgui(TEXTURE_SIZE, TEXTURE_SIZE);
		}
		ImGui::TreePop();
	}
}