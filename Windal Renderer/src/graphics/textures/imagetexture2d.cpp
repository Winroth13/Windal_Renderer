#include "core/logger.h"
#include "core/renderer.h"
#include "graphics/textures/imagetexture2d.h"

#include "stbImage/stb_image.h"
#include "imgui/imgui.h"

ImageTexture2D::ImageTexture2D(const std::string& path)
{
	mPath = path;

	int width, height, channels;
	char* imageData = LoadImageData(path, width, height, channels);

	if (imageData != nullptr)
	{
		Logger::Info("Loaded texture: " + path);
	}
	else
	{
		/* If texture couldn't be loaded, use error texture instead */
		imageData = LoadImageData("resources/fallback.png", width, height, channels);
		Logger::Warn("Failed to load texture from path: " + path + " (using fallback)");
		mPath += " <ERROR>";
	}

	mWidth = width;
	mHeight = height;
	mChannels = channels;

	/* Create Sampler */
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = Renderer::GetDevice()->CreateSamplerState(&samplerDesc, &mSamplerState);

	if (FAILED(hr))
	{
		Logger::Error("DirectX failed to create sampler state for texture: " + path);
		throw std::runtime_error("");
	}

	/* Create Texture */
	D3D11_TEXTURE2D_DESC desc = {};
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Width = mWidth;
	desc.Height = mHeight;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = imageData;
	data.SysMemPitch = mWidth * sizeof(char) * 4;
	data.SysMemSlicePitch = 0;

	hr = Renderer::GetDevice()->CreateTexture2D(&desc, nullptr, &mTexture);

	if (FAILED(hr))
	{
		Logger::Error("DirectX failed to create Texture2D for texture: " + path);
		throw std::runtime_error("");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC mipSrvDesc = {};
	memset(&mipSrvDesc, 0, sizeof(mipSrvDesc));
	mipSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mipSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	mipSrvDesc.Texture2D.MipLevels = -1;

	hr = Renderer::GetDevice()->CreateShaderResourceView(mTexture, &mipSrvDesc, &mShaderResourceView);

	if (FAILED(hr))
	{
		Logger::Error("DirectX failed to shader resource view for texture:" + path);
		throw std::runtime_error("");
	}

	UINT rowPitch = mWidth * sizeof(char) * 4;
	size_t imageSize = rowPitch * mHeight;

	Renderer::GetContext()->UpdateSubresource(mTexture, 0, nullptr, imageData, rowPitch, imageSize);
	Renderer::GetContext()->GenerateMips(mShaderResourceView);

	stbi_image_free(imageData);
}

ImageTexture2D::~ImageTexture2D()
{
}

void ImageTexture2D::RenderImgui(const uint32_t width, const uint32_t height)
{
	ImGui::Text("%s (%dx%d)", mPath.c_str(), mWidth, mHeight);
	ImGui::Image(
		(ImTextureID)(intptr_t)mShaderResourceView,
		ImVec2(static_cast<float>(width), static_cast<float>(height))
	);
}