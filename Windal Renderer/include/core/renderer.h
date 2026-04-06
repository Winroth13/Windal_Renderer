#pragma once
#include <array>
#include <d3d11.h>

#include "core/window.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Create(std::array<float, 4> clearColor, Window* window);
	void Shutdown();

	void BeginRender();
	void EndRender();

	static ID3D11Device* GetDevice() { return Renderer::mDevice; }

	ID3D11DeviceContext* GetContext() { return mImmediateContext; }

private:
	std::array<float, 4> mClearColor;
	Window* mWindow;

	static ID3D11Device* mDevice;
	ID3D11DeviceContext* mImmediateContext;

	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mRenderTargetView;

	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView;

	D3D11_VIEWPORT mViewport;
};