#include "core/renderer.h"
#include <iostream>
#include "core/logger.h"

Renderer::Renderer() :
	mImmediateContext(nullptr),
	mSwapChain(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilTexture(nullptr),
	mDepthStencilView(nullptr),
	mWindow(nullptr),
	mClearColor({ 0,0,0,255 })
{
}

ID3D11Device* Renderer::mDevice = nullptr;

Renderer::~Renderer()
{
}

bool Renderer::Create(std::array<float, 4> clearColor, Window* window)
{
	mClearColor = clearColor;

	if (window == nullptr)
	{
		Logger::Error("Renderer was given an invalid window");
		return false;
	}

	mWindow = window;

	if (mRenderServer.Create(this) == false)
	{
		Logger::Error("Renderer failed to create RenderServer");
		return false;
	}

	/* Create Interface */
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

		swapChainDesc.BufferDesc.Width = window->Width();
		swapChainDesc.BufferDesc.Height = window->Height();
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.OutputWindow = mWindow->GetWindowsWindow();
		swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.Flags = 0;

		UINT flags = 0;
		if (_DEBUG)
			flags = D3D11_CREATE_DEVICE_DEBUG;

		D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

		HRESULT hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			flags,
			featurelevels,
			1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&mSwapChain,
			&Renderer::mDevice,
			nullptr,
			&mImmediateContext
		);

		if (FAILED(hr))
		{
			Logger::Error("Failed to create device and swapchain");
			return false;
		}
	}

	/* Create Render Target View */
	{
		ID3D11Texture2D* backBuffer = nullptr;
		if (
			FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
		{
			Logger::Error("Failed to create render target");
			return false;
		}

		HRESULT hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
		backBuffer->Release();

		if (FAILED(hr))
		{
			Logger::Error("Failed to create render target view");
			return false;
		}
	}

	/* Create Depth Stencil	*/
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = window->Width();
		textureDesc.Height = window->Height();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		if (FAILED(mDevice->CreateTexture2D(&textureDesc, nullptr, &mDepthStencilTexture)))
		{
			Logger::Error("Failed to create depth stencil");
			return false;
		}

		HRESULT hr = mDevice->CreateDepthStencilView(
			mDepthStencilTexture,
			nullptr,
			&mDepthStencilView
		);

		if (FAILED(hr))
		{
			Logger::Error("Failed to create depth stencil view");
			return false;
		}
	}

	/* Set Viewport */
	{
		mViewport.TopLeftX = 0;
		mViewport.TopLeftY = 0;
		mViewport.Width = static_cast<float>(window->Width());
		mViewport.Height = static_cast<float>(window->Height());
		mViewport.MinDepth = 0;
		mViewport.MaxDepth = 1;
	}

	/* Create Constant Buffers */
	{
		/* Per Frame */
		{
			PerFrameBuffer perFrameBuffer = {};
			D3D11_BUFFER_DESC perFrameBufferDesc = {};
			perFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			perFrameBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			perFrameBufferDesc.CPUAccessFlags = 0;
			perFrameBufferDesc.MiscFlags = 0;
			perFrameBufferDesc.ByteWidth = sizeof(perFrameBuffer);
			perFrameBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &perFrameBuffer;

			HRESULT hr = mDevice->CreateBuffer(&perFrameBufferDesc, &csd, &mPerFrameBuffer);

			if (FAILED(hr))
			{
				Logger::Error("Failed to create per frame buffer");
				return false;
			}
		}

		/* Per Object */
		{
			PerObject perObjectBuffer = {};
			D3D11_BUFFER_DESC perObjectBufferDesc = {};
			perObjectBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			perObjectBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			perObjectBufferDesc.CPUAccessFlags = 0;
			perObjectBufferDesc.MiscFlags = 0;
			perObjectBufferDesc.ByteWidth = sizeof(perObjectBuffer);
			perObjectBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &perObjectBuffer;

			HRESULT hr = mDevice->CreateBuffer(&perObjectBufferDesc, &csd, &mPerObjectBuffer);

			if (FAILED(hr))
			{
				Logger::Error("Failed to create per object buffer");
				return false;
			}
		}

		/* Per View */
		{
			PerViewBuffer perViewBuffer = {};
			D3D11_BUFFER_DESC perViewBufferDesc = {};
			perViewBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			perViewBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			perViewBufferDesc.CPUAccessFlags = 0;
			perViewBufferDesc.MiscFlags = 0;
			perViewBufferDesc.ByteWidth = sizeof(perViewBuffer);
			perViewBufferDesc.StructureByteStride = 0;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &perViewBuffer;

			HRESULT hr = mDevice->CreateBuffer(&perViewBufferDesc, &csd, &mPerViewBuffer);

			if (FAILED(hr))
			{
				Logger::Error("Failed to create per view buffer");
				return false;
			}
		}
	}

	return true;
}

void Renderer::Shutdown()
{
	mPerViewBuffer->Release();
	mPerObjectBuffer->Release();
	mPerFrameBuffer->Release();

	mDepthStencilView->Release();
	mDepthStencilTexture->Release();

	mRenderTargetView->Release();
	mSwapChain->Release();
	mImmediateContext->Release();
	mDevice->Release();
}

void Renderer::BeginRender()
{
	mImmediateContext->ClearRenderTargetView(
		mRenderTargetView,
		mClearColor.data()
	);

	mImmediateContext->ClearDepthStencilView(
		mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1,
		0
	);

	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mImmediateContext->RSSetViewports(1, &mViewport);

	UpdatePerFrameBuffer();
	mImmediateContext->VSSetConstantBuffers(0, 1, &mPerFrameBuffer);
	mImmediateContext->PSSetConstantBuffers(0, 1, &mPerFrameBuffer);

	mImmediateContext->VSSetConstantBuffers(1, 1, &mPerViewBuffer);
	mImmediateContext->PSSetConstantBuffers(1, 1, &mPerViewBuffer);
}

void Renderer::EndRender()
{
	mSwapChain->Present(1, 0);
}

void Renderer::UpdatePerFrameBuffer()
{
	PerFrameBuffer perFrameBuffer = {};
	perFrameBuffer.ambientColor = { 1.0f, 1.0f, 1.0f };
	perFrameBuffer.sunDirection = { 0.0f, 1.0f, 0.0f };
	perFrameBuffer.sunColor = { 1.0f, 1.0f, 1.0f };
	mImmediateContext->UpdateSubresource(mPerFrameBuffer, 0, NULL, &perFrameBuffer, 0, 0);
}

void Renderer::UpdatePerViewBuffer(
	const DirectX::XMVECTOR& cameraPos,
	const DirectX::XMMATRIX& viewProj
)
{
	PerViewBuffer perViewBuffer = {};
	perViewBuffer.cameraPos = cameraPos;
	perViewBuffer.viewProj = viewProj;
	mImmediateContext->UpdateSubresource(mPerViewBuffer, 0, NULL, &perViewBuffer, 0, 0);
}

void Renderer::UpdatePerObjectBuffer(const DirectX::XMMATRIX& world)
{
	PerObject perObjectBuffer = {};
	perObjectBuffer.world = DirectX::XMMatrixTranspose(world);
	perObjectBuffer.worldInverseTranspose = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, world));
	mImmediateContext->UpdateSubresource(mPerObjectBuffer, 0, NULL, &perObjectBuffer, 0, 0);
}