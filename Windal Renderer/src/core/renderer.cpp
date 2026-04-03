#include "core/renderer.h"

Renderer::Renderer() :
	mDevice(nullptr),
	mImmediateContext(nullptr),
	mSwapChain(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilTexture(nullptr),
	mDepthStencilView(nullptr),
    mWindow(nullptr),
    mClearColor({0,0,0,255})
{
}

Renderer::~Renderer()
{

}

bool Renderer::Create(std::array<float, 4> clearColor, Window* window)
{
	mClearColor = clearColor;
	mWindow = window;

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
			&mDevice,
			nullptr,
			&mImmediateContext
		);

		if (FAILED(hr))
			return false;
	}

	/* Create Render Target View */
	{
		ID3D11Texture2D* backBuffer = nullptr;
		if (
			FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
		{
			return false;
		}

		HRESULT hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
		backBuffer->Release();

		if (FAILED(hr))
			return false;
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
			return false;
		}

		HRESULT hr = mDevice->CreateDepthStencilView(
			mDepthStencilTexture,
			nullptr,
			&mDepthStencilView
		);

		if (FAILED(hr))
			return false;
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

	return true;
}

void Renderer::Shutdown()
{
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
}

void Renderer::EndRender()
{
	mSwapChain->Present(1, 0);
}