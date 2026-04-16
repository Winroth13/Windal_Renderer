#pragma once
#include <d3d11.h>

#include "core/window.h"
#include "core/renderserver.h"

#include "graphics/materials/material.h"
#include "graphics/meshes/mesh.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"
#include "graphics/shaders/computeshader.h"

#include <DirectXMath.h>
#include <vector>

#define MAX_DIRECTIONAL_LIGHTS 8
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8

#define DIRECTIONAL_LIGHT_SLOT 0
#define POINT_LIGHT_SLOT 1
#define SPOT_LIGHT_SLOT 2
#define DIFFUSE_TEXTURE_SLOT 3

enum GBuffer
{
	GBUFFER_POSITION,
	GBUFFER_NORMAL,
	GBUFFER_COLOR,
	MAX_GBUFFERS
};

struct PerFrameBuffer
{
	DirectX::XMFLOAT3 ambientColor;
	uint32_t numDirectionalLights;
	uint32_t numPointLights;
	uint32_t numSpotLights;
	uint32_t useBlinnPhong;
	float pad;
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

struct DirectionalLightData
{
	DirectX::XMFLOAT3 direction;
	float intensity;
	DirectX::XMFLOAT3 color;
	float pad0;
};

struct PointLightData
{
	DirectX::XMFLOAT3 position;
	float attenuation;
	DirectX::XMFLOAT3 color;
	float intensity;
};

struct SpotLightData
{
	DirectX::XMFLOAT3 position;
	float attenuation;
	DirectX::XMFLOAT3 color;
	float intensity;
	DirectX::XMFLOAT3 direction;
	float angle;
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

struct EnviromentData
{
	DirectX::XMFLOAT3 ambientColor;
	bool useBlinnPhong;
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
	static ID3D11DeviceContext* GetContext() { return Renderer::mImmediateContext; }
	RenderServer& GetRenderServer() { return mRenderServer; }

	void UpdatePerFrameBuffer(
		const DirectX::XMFLOAT3 ambientColor,
		const uint32_t numDirectionalLights,
		const uint32_t numPointLights,
		const uint32_t numSpotLights,
		const bool useBlinnPhong
	);

	void UpdatePerViewBuffer(
		const DirectX::XMMATRIX& viewProj,
		const DirectX::XMFLOAT3& cameraPos
	);
	void UpdatePerObjectBuffer(const DirectX::XMMATRIX world);

	void PushGeometryData(const GeometryData& geometryData);
	void PushMaterialData(const MaterialData& materialData);

	void PushDirectionalLightData(const DirectionalLightData& directionalLightData);
	void PushPointLightData(const PointLightData& pointLightData);
	void PushSpotLightData(const SpotLightData& spotLightData);

	void SetEnviromentData(const EnviromentData& enviromentData);

private:
	/* Bind Functions*/
	void BindGBuffers();
	void UnbindGBuffers();

	void BindMaterial(std::shared_ptr<Material> material);
	void BindMesh(std::shared_ptr<Mesh> mesh);
	void BindVertexShader(std::shared_ptr<VertexShader> vertexShader);
	void BindPixelShader(std::shared_ptr<PixelShader> pixelShader);
	void BindTexture2D(std::shared_ptr<Texture2D> texture2d, UINT slot);

	void BindDirectionalLights();
	void BindPointLights();
	void BindSpotLights();

	/* Unbind Functions */
	void UnbindMaterial();
	void UnbindMesh();
	void UnbindVertexShader();
	void UnbindPixelShader();
	void UnbindTexture2D(UINT slot);

	void ClearFrameData();

	std::vector<GeometryData> mGeometryData;
	std::vector<MaterialData> mMaterialData;
	EnviromentData mEnviromentData;

	/* Lights */
	std::vector<PointLightData> mPointLightsData;
	std::vector<SpotLightData> mSpotLightsData;
	std::vector<DirectionalLightData> mDirectionalLightsData;

	DirectX::XMFLOAT4 mClearColor;
	Window* mWindow;

	static ID3D11Device* sDevice;
	static ID3D11DeviceContext* mImmediateContext;

	std::unique_ptr<ComputeShader> mLightingComputeShader;

	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mBackBufferRenderTargetView;

	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView;

	D3D11_VIEWPORT mViewport;

	RenderServer mRenderServer;

	std::vector<ID3D11Texture2D*> mGBufferTextures;
	std::vector<ID3D11ShaderResourceView*> mGBufferResourceViews;
	std::vector<ID3D11RenderTargetView*> mGBufferRenderTargetViews;

	ID3D11Buffer* mPerFrameBuffer;
	ID3D11Buffer* mPerViewBuffer;
	ID3D11Buffer* mPerObjectBuffer;

	ID3D11Buffer* mDirectionalLightsBuffer;
	ID3D11ShaderResourceView* mDirectionalLightsSRV;

	ID3D11Buffer* mPointLightsBuffer;
	ID3D11ShaderResourceView* mPointLightsSRV;

	ID3D11Buffer* mSpotLightsBuffer;
	ID3D11ShaderResourceView* mSpotLightsSRV;
};