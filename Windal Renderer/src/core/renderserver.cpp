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

void RenderServer::PushPointLight(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float range, float intensity)
{
	PointLightData data = {};
	data.position = position;
	data.color = color;
	data.range = range;
	data.intensity = intensity;
	mRenderer->PushPointLightData(data);
}

void RenderServer::UpdateCamera(const DirectX::XMMATRIX viewProj, const DirectX::XMFLOAT3 cameraPos)
{
	mRenderer->UpdatePerViewBuffer(viewProj, cameraPos);
}

void RenderServer::UpdateEnviroment(Enviroment& enviroment)
{
	EnviromentData data = {};
	data.ambientColor = enviroment.GetAmbientColor();
	data.sunColor = enviroment.GetSunColor();
	data.sunDirection = enviroment.GetSunDirection();
	mRenderer->SetEnviromentData(data);
}
