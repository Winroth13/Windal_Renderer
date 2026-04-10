#pragma once
#include <d3d11.h>

#include "core/window.h"
#include "core/renderserver.h"

#include "graphics/materials/material.h"
#include "graphics/meshes/mesh.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"

#include <DirectXMath.h>
#include <vector>

struct PerFrameBuffer
{
	DirectX::XMFLOAT3 sunDirection;
	float pad0;
	DirectX::XMFLOAT3 sunColor;
	float pad1;
	DirectX::XMFLOAT3 ambientColor;
	float pad2;
};

struct PerViewBuffer
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMFLOAT3 cameraPos;
	float pad;
};

struct PerObject
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInverseTranspose;
};

struct MaterialData
{
	std::shared_ptr<Material> material;
};

struct GeometryData
{
	std::shared_ptr<Mesh> mesh;
	DirectX::XMMATRIX transform;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Create(DirectX::XMFLOAT4 clearColor, Window* window);
	void Shutdown();

	void BeginRender();
	void Render();
	void EndRender();

	static ID3D11Device* GetDevice() { return Renderer::sDevice; }
	ID3D11DeviceContext* GetContext() { return mImmediateContext; }
	RenderServer& GetRenderServer() { return mRenderServer; }

	void UpdatePerFrameBuffer(
		const DirectX::XMFLOAT3 ambientColor,
		const DirectX::XMFLOAT3 sunColor,
		const DirectX::XMFLOAT3 sunDirection
	);

	void UpdatePerViewBuffer(
		const DirectX::XMMATRIX& viewProj,
		const DirectX::XMFLOAT3& cameraPos
	);
	void UpdatePerObjectBuffer(const DirectX::XMMATRIX world);

	void PushFrameGeometryData(const GeometryData& geometryData);
	void PushFrameMaterialData(const MaterialData& materialData);

private:
	/* Bind Functions*/
	void BindMaterial(std::shared_ptr<Material> material);
	void BindMesh(std::shared_ptr<Mesh> mesh);
	void BindVertexShader(std::shared_ptr<VertexShader> vertexShader);
	void BindPixelShader(std::shared_ptr<PixelShader> pixelShader);
	void BindTexture2D(std::shared_ptr<Texture2D> texture2d, UINT slot);

	/* Unbind Functions */
	void UnbindMaterial();
	void UnbindMesh();
	void UnbindVertexShader();
	void UnbindPixelShader();
	void UnbindTexture2D(UINT slot);

	void ClearFrameData();

	std::vector<GeometryData> mFrameGeometryData;
	std::vector<MaterialData> mFrameMaterialData;

	DirectX::XMFLOAT4 mClearColor;
	Window* mWindow;

	static ID3D11Device* sDevice;
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