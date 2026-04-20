#include "core/renderserver.h"
#include "core/logger.h"
#include "core/renderer.h"
#include <iostream>
#include <d3d11.h>

#include "graphics/models/model.h"
#include "graphics/enviroment.h"

#include "core/transform.h"

bool RenderServer::Create(Renderer* renderer)
{
	mRenderer = renderer;
	if (renderer == nullptr)
	{
		Logger::Error("Failed to create RenderServer because renderer was invalid");
		return false;
	}
	return true;
}

void RenderServer::PushMesh(std::shared_ptr<Mesh> mesh, DirectX::XMMATRIX transform)
{
	GeometryData data = {};
	data.mesh = mesh;
	data.transform = transform;
	mRenderer->PushGeometryData(data);
}

void RenderServer::PushMaterial(std::shared_ptr<Material> material)
{
	MaterialData data = {};
	data.material = material;
	mRenderer->PushMaterialData(data);
}

void RenderServer::PushDirectionalLight(DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color, float intensity)
{
	DirectionalLightData data = {};
	data.direction = direction;
	data.color = color;
	data.intensity = intensity;
	mRenderer->PushDirectionalLightData(data);
}

void RenderServer::PushPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float attenuation, float intensity)
{
	PointLightData data = {};
	data.position = position;
	data.color = color;
	data.attenuation = attenuation;
	data.intensity = intensity;
	mRenderer->PushPointLightData(data);
}

void RenderServer::PushSpotLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 color, float angle, float intensity, float attenuation)
{
	SpotLightData data = {};
	data.position = position;
	data.direction = direction;
	data.color = color;
	data.angle = angle;
	data.intensity = intensity;
	data.attenuation = attenuation;
	mRenderer->PushSpotLightData(data);
}

void RenderServer::UpdateCamera(const DirectX::XMMATRIX viewProj, const DirectX::XMFLOAT3 cameraPos)
{
	CameraData data = {};
	data.viewProj = viewProj;
	data.pos = cameraPos;
	mRenderer->SetSceneCamera(data);
}

void RenderServer::UpdateEnviroment(Enviroment& enviroment)
{
	EnviromentData data = {};
	data.ambientColor = enviroment.GetAmbientColor();
	data.useBlinnPhong = enviroment.IsUsingBlinnPhong();
	mRenderer->SetEnviromentData(data);
}

void RenderServer::SetWireframe(bool value)
{
	if (value)
	{
		mRenderer->SetFlags(mRenderer->GetFlags() | WIRE_FRAME);
	}
	else
	{
		mRenderer->SetFlags(mRenderer->GetFlags() & ~WIRE_FRAME);
	}
}

void RenderServer::SetShowGBuffer(bool value)
{
	if (value)
	{
		mRenderer->SetFlags(mRenderer->GetFlags() | SHOW_GBUFFERS);
	}
	else
	{
		mRenderer->SetFlags(mRenderer->GetFlags() & ~SHOW_GBUFFERS);
	}
}