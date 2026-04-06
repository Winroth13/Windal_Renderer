#pragma once
#include <array>
#include <d3d11.h>

#include "core/window.h"
#include "core/renderserver.h"
#include <DirectXMath.h>

struct PerFrameBuffer
{
	DirectX::XMVECTOR sunDirection;
	DirectX::XMVECTOR sunColor;
	DirectX::XMVECTOR ambientColor;
};

struct PerViewBuffer
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMVECTOR cameraPos;
};

struct PerObject
{
	DirectX::XMMATRIX worldViewProj;
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInverseTranspose;
};

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
	RenderServer& GetRenderServer() { return mRenderServer; }

	void UpdatePerFrameBuffer();
	void UpdatePerViewBuffer(
		const DirectX::XMVECTOR& cameraPos,
		const DirectX::XMMATRIX& viewProj
	);
	void UpdatePerObjectBuffer(const DirectX::XMMATRIX& world);

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

	RenderServer mRenderServer;

	ID3D11Buffer* mPerFrameBuffer;
	ID3D11Buffer* mPerViewBuffer;
	ID3D11Buffer* mPerObjectBuffer;
};