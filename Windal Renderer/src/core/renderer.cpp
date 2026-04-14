#include "core/renderer.h"
#include <iostream>
#include "core/logger.h"

#include <DirectXMath.h>

#include "graphics/camera.h"

Renderer::Renderer() :
	mImmediateContext(nullptr),
	mSwapChain(nullptr),
	mRenderTargetView(nullptr),
	mDepthStencilTexture(nullptr),
	mDepthStencilView(nullptr),
	mWindow(nullptr),
	mClearColor({ 0,0,0,255 }),
	mPerFrameBuffer(nullptr),
	mPerObjectBuffer(nullptr),
	mPerViewBuffer(nullptr),
	mViewport({})
{
}

ID3D11Device* Renderer::sDevice = nullptr;

Renderer::~Renderer()
{
}

bool Renderer::Create(DirectX::XMFLOAT4 clearColor, Window* window)
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
			&Renderer::sDevice,
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

		HRESULT hr = sDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
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

		if (FAILED(sDevice->CreateTexture2D(&textureDesc, nullptr, &mDepthStencilTexture)))
		{
			Logger::Error("Failed to create depth stencil");
			return false;
		}

		HRESULT hr = sDevice->CreateDepthStencilView(
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

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = &perFrameBuffer;

			HRESULT hr = sDevice->CreateBuffer(&perFrameBufferDesc, &data, &mPerFrameBuffer);

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

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = &perObjectBuffer;

			HRESULT hr = sDevice->CreateBuffer(&perObjectBufferDesc, &data, &mPerObjectBuffer);

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

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = &perViewBuffer;

			HRESULT hr = sDevice->CreateBuffer(&perViewBufferDesc, &data, &mPerViewBuffer);

			if (FAILED(hr))
			{
				Logger::Error("Failed to create per view buffer");
				return false;
			}
		}
	}

	/* Create Directional Light Buffers */
	{
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.ByteWidth = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS;
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		lightBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		lightBufferDesc.StructureByteStride = sizeof(DirectionalLightData);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = 0;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(sDevice->CreateBuffer(&lightBufferDesc, NULL, &mDirectionalLightsBuffer)))
		{
			Logger::Error("Failed to create directional lights buffer");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = MAX_DIRECTIONAL_LIGHTS;

		if (FAILED(sDevice->CreateShaderResourceView(mDirectionalLightsBuffer, &srvDesc, &mDirectionalLightsSRV)))
		{
			Logger::Error("Failed to create directional lights shader resource view");
			return false;
		}

		mDirectionalLightsData.reserve(MAX_DIRECTIONAL_LIGHTS);
	}

	/* Create Point Light Buffers */
	{
		D3D11_BUFFER_DESC pointLightsBufferDesc = {};
		pointLightsBufferDesc.ByteWidth = sizeof(PointLightData) * MAX_POINT_LIGHTS;
		pointLightsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		pointLightsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		pointLightsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		pointLightsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		pointLightsBufferDesc.StructureByteStride = sizeof(PointLightData);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = 0;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(sDevice->CreateBuffer(&pointLightsBufferDesc, NULL, &mPointLightsBuffer)))
		{
			Logger::Error("Failed to create point lights buffer");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = MAX_POINT_LIGHTS;

		if (FAILED(sDevice->CreateShaderResourceView(mPointLightsBuffer, &srvDesc, &mPointLightsSRV)))
		{
			Logger::Error("Failed to create point lights shader resource view");
			return false;
		}

		mPointLightsData.reserve(MAX_POINT_LIGHTS);
	}

	/* Create Spot Light Buffers */
	{
		D3D11_BUFFER_DESC lightBufferDesc = {};
		lightBufferDesc.ByteWidth = sizeof(SpotLightData) * MAX_SPOT_LIGHTS;
		lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		lightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		lightBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		lightBufferDesc.StructureByteStride = sizeof(SpotLightData);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = 0;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		if (FAILED(sDevice->CreateBuffer(&lightBufferDesc, NULL, &mSpotLightsBuffer)))
		{
			Logger::Error("Failed to create spot lights buffer");
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = MAX_SPOT_LIGHTS;

		if (FAILED(sDevice->CreateShaderResourceView(mSpotLightsBuffer, &srvDesc, &mSpotLightsSRV)))
		{
			Logger::Error("Failed to create spot lights shader resource view");
			return false;
		}

		mSpotLightsData.reserve(MAX_SPOT_LIGHTS);
	}

	return true;
}

void Renderer::Shutdown()
{
	/* Release Light Buffers */
	{
		if (mDirectionalLightsBuffer != nullptr)
			mDirectionalLightsBuffer->Release();

		if (mDirectionalLightsSRV != nullptr)
			mDirectionalLightsSRV->Release();

		if (mPointLightsBuffer != nullptr)
			mPointLightsBuffer->Release();

		if (mPointLightsSRV != nullptr)
			mPointLightsSRV->Release();

		if (mSpotLightsBuffer != nullptr)
			mSpotLightsBuffer->Release();

		if (mSpotLightsSRV != nullptr)
			mSpotLightsSRV->Release();
	}

	if (mPerViewBuffer != nullptr)
		mPerViewBuffer->Release();

	if (mPerObjectBuffer != nullptr)
		mPerObjectBuffer->Release();

	if (mPerFrameBuffer != nullptr)
		mPerFrameBuffer->Release();

	if (mDepthStencilView != nullptr)
		mDepthStencilView->Release();

	if (mDepthStencilTexture != nullptr)
		mDepthStencilTexture->Release();

	if (mRenderTargetView != nullptr)
		mRenderTargetView->Release();

	if (mSwapChain != nullptr)
		mSwapChain->Release();

	if (mImmediateContext != nullptr)
		mImmediateContext->Release();

	if (sDevice != nullptr)
		sDevice->Release();
}

void Renderer::BeginRender()
{
	mImmediateContext->ClearRenderTargetView(
		mRenderTargetView,
		&mClearColor.x
	);

	mImmediateContext->ClearDepthStencilView(
		mDepthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1,
		0
	);

	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mImmediateContext->RSSetViewports(1, &mViewport);

	mImmediateContext->VSSetConstantBuffers(0, 1, &mPerFrameBuffer);
	mImmediateContext->PSSetConstantBuffers(0, 1, &mPerFrameBuffer);

	mImmediateContext->VSSetConstantBuffers(1, 1, &mPerViewBuffer);
	mImmediateContext->PSSetConstantBuffers(1, 1, &mPerViewBuffer);
}

void Renderer::Render()
{
	UpdatePerFrameBuffer(
		mEnviromentData.ambientColor,
		(uint32_t)mDirectionalLightsData.size(),
		(uint32_t)mPointLightsData.size(),
		(uint32_t)mSpotLightsData.size(),
		mEnviromentData.useBlinnPhong
	);

	BindDirectionalLights();
	BindPointLights();
	BindSpotLights();

	/* Draw each set of Mesh and Material Data */
	for (size_t i = 0; i < mGeometryData.size(); ++i)
	{
		auto& mesh = mGeometryData[i].mesh;
		auto& mat = mMaterialData[i].material;

		BindMesh(mesh);
		BindMaterial(mat);

		UpdatePerObjectBuffer(mGeometryData[i].transform);

		mImmediateContext->DrawIndexed((UINT)mesh->GetNumIndicies(), 0, 0);
	}
}

void Renderer::EndRender()
{
	ClearFrameData();
	mSwapChain->Present(1, 0);
}

void Renderer::UpdatePerFrameBuffer(
	const DirectX::XMFLOAT3 ambientColor,
	const uint32_t numDirectionalLights,
	const uint32_t numPointLights,
	const uint32_t numSpotLights,
	bool useBlinnPhong)
{
	PerFrameBuffer perFrameBuffer = {};
	perFrameBuffer.ambientColor = ambientColor;

	perFrameBuffer.numDirectionalLights = numDirectionalLights;
	perFrameBuffer.numPointLights = numPointLights;
	perFrameBuffer.numSpotLights = numSpotLights;

	perFrameBuffer.useBlinnPhong = (uint32_t)useBlinnPhong;

	mImmediateContext->UpdateSubresource(mPerFrameBuffer, 0, NULL, &perFrameBuffer, 0, 0);
}

void Renderer::UpdatePerViewBuffer(
	const DirectX::XMMATRIX& viewProj,
	const DirectX::XMFLOAT3& cameraPos
)
{
	PerViewBuffer perViewBuffer = {};
	perViewBuffer.cameraPos = cameraPos;
	perViewBuffer.viewProj = viewProj;
	mImmediateContext->UpdateSubresource(mPerViewBuffer, 0, NULL, &perViewBuffer, 0, 0);
}

void Renderer::UpdatePerObjectBuffer(const DirectX::XMMATRIX world)
{
	PerObject perObjectBuffer = {};
	perObjectBuffer.world = DirectX::XMMatrixTranspose(world);
	perObjectBuffer.worldInverseTranspose = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, world));
	mImmediateContext->UpdateSubresource(mPerObjectBuffer, 0, NULL, &perObjectBuffer, 0, 0);

	mImmediateContext->VSSetConstantBuffers(BUFFER_PER_OBJECT, 1, &mPerObjectBuffer);
	mImmediateContext->PSSetConstantBuffers(BUFFER_PER_OBJECT, 1, &mPerObjectBuffer);
}

void Renderer::PushGeometryData(const GeometryData& geometryData)
{
	mGeometryData.push_back(geometryData);
}

void Renderer::PushMaterialData(const MaterialData& materialData)
{
	mMaterialData.push_back(materialData);
}

void Renderer::PushDirectionalLightData(const DirectionalLightData& directionalLightData)
{
	if (mDirectionalLightsData.size() < MAX_DIRECTIONAL_LIGHTS)
	{
		mDirectionalLightsData.push_back(directionalLightData);
	}
	else
	{
		Logger::Warn("Directional lights cannot exceed " + std::to_string(MAX_DIRECTIONAL_LIGHTS));
	}
}

void Renderer::PushPointLightData(const PointLightData& pointLightData)
{
	if (mPointLightsData.size() < MAX_POINT_LIGHTS)
	{
		mPointLightsData.push_back(pointLightData);
	}
	else
	{
		Logger::Warn("Point lights cannot exceed " + std::to_string(MAX_POINT_LIGHTS));
	}
}

void Renderer::PushSpotLightData(const SpotLightData& spotLightData)
{
	if (mSpotLightsData.size() < MAX_SPOT_LIGHTS)
	{
		mSpotLightsData.push_back(spotLightData);
	}
	else
	{
		Logger::Warn("Spot lights cannot exceed " + std::to_string(MAX_SPOT_LIGHTS));
	}
}

void Renderer::SetEnviromentData(const EnviromentData& enviromentData)
{
	mEnviromentData = enviromentData;
}

void Renderer::BindMaterial(std::shared_ptr<Material> material)
{
	BindVertexShader(material->GetVertexShader());
	BindPixelShader(material->GetPixelShader());

	BindTexture2D(material->GetTexture(), DIFFUSE_TEXTURE_SLOT);

	mImmediateContext->IASetInputLayout(material->GetInputLayout());

	ID3D11Buffer* buffer = material->GetBuffer(mImmediateContext);
	mImmediateContext->PSSetConstantBuffers(BUFFER_PER_MATERIAL, 1, &buffer);
}

void Renderer::BindMesh(std::shared_ptr<Mesh> mesh)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffer = mesh->GetVertexBuffer();
	ID3D11Buffer* indexBuffer = mesh->GetIndexBuffer();

	mImmediateContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	mImmediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::BindVertexShader(std::shared_ptr<VertexShader> vertexShader)
{
	mImmediateContext->VSSetShader(
		vertexShader->GetShader(),
		nullptr,
		0
	);
}

void Renderer::BindPixelShader(std::shared_ptr<PixelShader> pixelShader)
{
	mImmediateContext->PSSetShader(
		pixelShader->GetShader(),
		nullptr,
		0
	);
}

void Renderer::BindTexture2D(std::shared_ptr<Texture2D> texture2d, UINT slot)
{
	ID3D11SamplerState* sampler = texture2d->GetSamplerState();
	ID3D11ShaderResourceView* srv = texture2d->GetSRV();

	mImmediateContext->PSSetSamplers(slot, 1, &sampler);
	mImmediateContext->PSSetShaderResources(slot, 1, &srv);
}

void Renderer::BindDirectionalLights()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = mImmediateContext->Map(mDirectionalLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Logger::Error("Failed to map directional lights buffer");
		throw std::runtime_error("");
	}

	size_t numBytes = mDirectionalLightsData.size() * sizeof(DirectionalLightData);
	memcpy_s(mappedResource.pData, numBytes, mDirectionalLightsData.data(), numBytes);
	mImmediateContext->Unmap(mDirectionalLightsBuffer, 0);

	mImmediateContext->PSSetShaderResources(DIRECTIONAL_LIGHT_SLOT, 1, &mDirectionalLightsSRV);
}

void Renderer::BindPointLights()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = mImmediateContext->Map(mPointLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Logger::Error("Failed to map point lights buffer");
		throw std::runtime_error("");
	}

	size_t numBytes = mPointLightsData.size() * sizeof(PointLightData);
	memcpy_s(mappedResource.pData, numBytes, mPointLightsData.data(), numBytes);
	mImmediateContext->Unmap(mPointLightsBuffer, 0);

	mImmediateContext->PSSetShaderResources(POINT_LIGHT_SLOT, 1, &mPointLightsSRV);
}

void Renderer::BindSpotLights()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = mImmediateContext->Map(mSpotLightsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		Logger::Error("Failed to map spot lights buffer");
		throw std::runtime_error("");
	}

	size_t numBytes = mSpotLightsData.size() * sizeof(SpotLightData);
	memcpy_s(mappedResource.pData, numBytes, mSpotLightsData.data(), numBytes);
	mImmediateContext->Unmap(mSpotLightsBuffer, 0);

	mImmediateContext->PSSetShaderResources(SPOT_LIGHT_SLOT, 1, &mSpotLightsSRV);
}

void Renderer::UnbindMaterial()
{
	UnbindVertexShader();
	UnbindPixelShader();
}

void Renderer::UnbindMesh()
{
	mImmediateContext->IASetVertexBuffers(0, 1, nullptr, 0, nullptr);
	mImmediateContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
}

void Renderer::UnbindVertexShader()
{
	mImmediateContext->VSSetShader(nullptr, nullptr, 0);
}

void Renderer::UnbindPixelShader()
{
	mImmediateContext->PSSetShader(nullptr, nullptr, 0);
}

void Renderer::UnbindTexture2D(UINT slot)
{
	mImmediateContext->PSSetSamplers(slot, 1, nullptr);
	mImmediateContext->PSSetShaderResources(slot, 1, nullptr);
}

void Renderer::ClearFrameData()
{
	/* Lights */
	{
		mDirectionalLightsData.clear();
		mSpotLightsData.clear();
		mPointLightsData.clear();
	}

	mGeometryData.clear();
	mMaterialData.clear();
}