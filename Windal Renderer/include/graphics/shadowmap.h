#pragma once
#include <vector>
#include <d3d11.h>

#define SHADOW_MAP_WIDTH 2048
#define	SHADOW_MAP_HEIGHT 2048

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	bool Create(size_t maxNumMaps);

	ID3D11ShaderResourceView* GetSRV(size_t numMaps);
	ID3D11DepthStencilView* GetDSV(size_t i) { return mDepthStencilViews[i]; }

private:
	size_t mMaxMaps = 0;

	ID3D11Texture2D* mDepthTextureArray = nullptr;
	ID3D11DepthStencilView** mDepthStencilViews;

	ID3D11ShaderResourceView* mSrv = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC mSrvDesc = {};
};