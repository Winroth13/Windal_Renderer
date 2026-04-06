#pragma once
#include <string>
#include <iostream>
#include <d3d11.h>

class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();

	char* LoadImageData(
		const std::string& path, 
		int& width, 
		int& height, 
		int& channels, 
		int desiredChannels = 4
	);

	inline ID3D11ShaderResourceView* GetSRV() { return mShaderResourceView; }
	inline ID3D11SamplerState* GetSamplerState() { return mSamplerState; }

	virtual uint32_t GetWidth() = 0;
	virtual uint32_t GetHeight() = 0;

protected:
	ID3D11Texture2D* mTexture = nullptr;
	ID3D11SamplerState* mSamplerState = nullptr;
	ID3D11ShaderResourceView* mShaderResourceView = nullptr;
};