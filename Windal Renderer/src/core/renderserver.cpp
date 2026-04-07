#include "core/renderserver.h"
#include "core/logger.h"
#include "core/renderer.h"
#include <iostream>
#include <d3d11.h>

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

void RenderServer::RenderIndexed(size_t numIndicies)
{
	GetContext()->DrawIndexed((UINT)numIndicies, 0, 0);
	Logger::Info("Rendering entity");
}