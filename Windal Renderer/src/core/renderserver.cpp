#include "core/renderserver.h"
#include "core/logger.h"
#include "core/renderer.h"
#include <iostream>
#include <d3d11.h>

#include "graphics/models/model.h"
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

ID3D11DeviceContext* RenderServer::GetContext()
{
	return mRenderer->GetContext();
}

void RenderServer::RenderModel(std::shared_ptr<Model> model, Transform& transform)
{
	model->Bind(*this);
	mRenderer->UpdatePerObjectBuffer(transform.GetMatrix());
	GetContext()->DrawIndexed((UINT)model->GetNumIndicies(), 0, 0);
}

void RenderServer::UpdateCamera(const DirectX::XMMATRIX viewProj, const DirectX::XMFLOAT3 cameraPos)
{
	mRenderer->UpdatePerViewBuffer(viewProj, cameraPos);
}
