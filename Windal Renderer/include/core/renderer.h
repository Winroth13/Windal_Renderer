#pragma once
#include <d3d11.h>

#include "core/window.h"
#include "core/renderserver.h"

#include "graphics/materials/material.h"
#include "graphics/meshes/mesh.h"
#include "graphics/shaders/vertexshader.h"
#include "graphics/shaders/pixelshader.h"
#include "graphics/shaders/computeshader.h"
#include "graphics/shadowmap.h"
#include "core/transform.h"

#include <DirectXMath.h>
#include <vector>
#include <array>

#define MAX_DIRECTIONAL_LIGHTS 8
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 8

#define MAX_MATERIALS 64

#define DIRECTIONAL_LIGHT_SLOT 0
#define POINT_LIGHT_SLOT 1
#define SPOT_LIGHT_SLOT 2

#define DIRECTIONAL_LIGHT_SHADOW_MAPS_SLOT 4
#define POINT_LIGHT_SHADOW_MAPS_SLOT 5
#define SPOT_LIGHT_SHADOW_MAPS_SLOT 6

#define DIFFUSE_TEXTURE_SLOT 3

#define DEFERRED_MATERIALS_SLOT 3
#define TEXTURE_SAMPLER_SLOT 3

#define GBUFFER_START_SLOT 7

#define DEFAULT_SAMPLER_SLOT 0
#define SHADOW_MAP_SAMPLER_SLOT 1

enum RenderFlags
{
	WIRE_FRAME = 1,
	SHOW_GBUFFERS = 2,
	USE_BLINN_PHONG = 4
};

enum GBuffer
{
	GBUFFER_POSITION,
	GBUFFER_NORMAL,
	GBUFFER_COLOR,
	MAX_GBUFFERS
};

enum class ShaderType
{
	VERTEX,
	PIXEL,
	COMPUTE
};

struct PerFrameBuffer
{
	DirectX::XMFLOAT3 ambientColor;
	uint32_t numDirectionalLights;
	uint32_t numPointLights;
	uint32_t numSpotLights;
	uint32_t flags;
	float pad0;
	std::array<uint32_t, 2> screenDimensions;
	DirectX::XMFLOAT2 pad1;
};

struct PerViewBuffer
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMFLOAT3 cameraPos;
	float pad0;
};

struct PerObject
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInverseTranspose;
};

struct PerMaterial
{
	DirectX::XMFLOAT3 ambientCoefficient;
	float pad0;
	DirectX::XMFLOAT3 diffuseCoefficient;
	float pad1;
	DirectX::XMFLOAT3 specularCoefficient;
	float phongExponent;
};

struct MaterialIndexBuffer
{
	uint32_t materialIndex;
	DirectX::XMFLOAT3 pad0;
};

struct DirectionalLightData
{
	DirectX::XMMATRIX viewProj;
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
	DirectX::XMMATRIX viewProj;
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

struct CameraData
{
	DirectX::XMMATRIX viewProj;
	DirectX::XMFLOAT3 pos;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Create(DirectX::XMFLOAT4 clearColor, Window* window);
	void Shutdown();

	void BeginRender();

	void RenderShadowMaps();
	void RenderDeferred();

	void BeginForward();
	void RenderForward();
	void EndForward();

	void PresentRender();

	static ID3D11Device* GetDevice() { return Renderer::sDevice; }
	static ID3D11DeviceContext* GetContext() { return Renderer::mImmediateContext; }
	RenderServer& GetRenderServer() { return mRenderServer; }

	void UpdatePerFrameBuffer(
		const DirectX::XMFLOAT3 ambientColor,
		const uint32_t numDirectionalLights,
		const uint32_t numPointLights,
		const uint32_t numSpotLights,
		const uint32_t flags,
		const std::array<uint32_t, 2> screenDimensions
	);

	void PushGeometryData(const GeometryData& geometryData);
	void PushMaterialData(const MaterialData& materialData);

	void PushDirectionalLightData(const DirectionalLightData& directionalLightData);
	void PushPointLightData(const PointLightData& pointLightData);
	void PushSpotLightData(const SpotLightData& spotLightData);

	void SetEnviromentData(const EnviromentData& enviromentData);
	void SetSceneCamera(const CameraData& cameraData);

	const uint16_t GetFlags() { return mNewFlags; }
	void SetFlags(const uint16_t flags) { mNewFlags = flags; }

private:
	void UpdatePerViewBuffer(const CameraData& cameraData);
	void UpdatePerObjectBuffer(const DirectX::XMMATRIX world);

	/* Bind Functions*/
	void BindGBuffers();
	void UnbindGBuffers();

	void BindPerFrameBuffer(ShaderType shaderType);
	void BindPerViewBuffer(ShaderType shaderType);

	void BindMaterial(std::shared_ptr<Material> material, uint32_t index);
	void BindMesh(std::shared_ptr<Mesh> mesh);

	void BindVertexShader(std::shared_ptr<VertexShader> vertexShader);
	void BindVertexShader(const std::unique_ptr<VertexShader>& vertexShader);

	void BindPixelShader(std::shared_ptr<PixelShader> pixelShader);
	void BindPixelShader(const std::unique_ptr<PixelShader>& pixelShader);

	void BindTexture2D(std::shared_ptr<Texture2D> texture2d, UINT slot);

	void BindDirectionalLights(ShaderType shaderType);
	void BindPointLights(ShaderType shaderType);
	void BindSpotLights(ShaderType shaderType);

	/* Unbind Functions */
	void UnbindMaterial();
	void UnbindMesh();
	void UnbindVertexShader();
	void UnbindPixelShader();
	void UnbindTexture2D(UINT slot);

	/* Update Buffers */
	void UpdateMaterialIndexBuffer(uint32_t index);

	void ClearFrameData();

	/* Math */
	DirectX::XMFLOAT3 DirectionToAngles(DirectX::XMFLOAT3 direction);

	std::vector<GeometryData> mGeometryData;
	std::vector<MaterialData> mMaterialData;

	EnviromentData mEnviromentData;
	CameraData mSceneCamera; // TODO: Maybe this should have a better name...

	/* Lights */
	std::vector<DirectionalLightData> mDirectionalLightsData;
	std::vector<PointLightData> mPointLightsData;
	std::vector<SpotLightData> mSpotLightsData;

	/* Shadow Mapping */
	std::unique_ptr<VertexShader> mShadowMapVertexShader;
	ShadowMap mDirectionalLightsShadowMap;
	ShadowMap mSpotLightsShadowMap;
	ID3D11InputLayout* mShadowInputLayout = nullptr;
	ID3D11SamplerState* mShadowMapSampler = nullptr;

	DirectX::XMFLOAT4 mClearColor;
	Window* mWindow;

	static ID3D11Device* sDevice;
	static ID3D11DeviceContext* mImmediateContext;

	std::unique_ptr<ComputeShader> mLightingComputeShader;
	std::unique_ptr<PixelShader> mDeferredPixelShader;

	IDXGISwapChain* mSwapChain;
	ID3D11RenderTargetView* mBackBufferRenderTargetView;
	ID3D11UnorderedAccessView* mBackBufferUAV;

	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView;

	D3D11_VIEWPORT mViewport;

	RenderServer mRenderServer;

	std::vector<ID3D11Texture2D*> mGBufferTextures;
	std::vector<ID3D11ShaderResourceView*> mGBufferResourceViews;
	std::vector<ID3D11RenderTargetView*> mGBufferRenderTargetViews;

	ID3D11SamplerState* mDefaultSampler;

	ID3D11Buffer* mPerFrameBuffer;
	ID3D11Buffer* mPerViewBuffer;
	ID3D11Buffer* mPerObjectBuffer;

	ID3D11Buffer* mMaterialIndexBuffer;

	ID3D11Buffer* mDirectionalLightsBuffer;
	ID3D11ShaderResourceView* mDirectionalLightsSRV;

	ID3D11Buffer* mPointLightsBuffer;
	ID3D11ShaderResourceView* mPointLightsSRV;

	ID3D11Buffer* mSpotLightsBuffer;
	ID3D11ShaderResourceView* mSpotLightsSRV;

	ID3D11Buffer* mMaterialsBuffer;
	ID3D11ShaderResourceView* mMaterialsSRV;

	uint16_t mFlags = 0;
	uint16_t mNewFlags = 0;
};