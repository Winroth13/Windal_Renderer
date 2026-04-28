#include "core/entities/cubemapentity.h"
#include "graphics/textures/cubemaptexture.h"

CubemapEntity::CubemapEntity(uint32_t dimensions)
	: Entity("Cubemap")
{
	mCubemapTexture = std::make_shared<CubemapTexture>(dimensions, dimensions);
}

CubemapEntity::~CubemapEntity()
{
}

void CubemapEntity::UpdateSelf(double delta)
{
}

void CubemapEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.PushCubemap(GetGlobalPosition(), mCubemapTexture);

	// TODO: Makes the cubemap only render once, remove before handing in!
	if (mVisible)
	{
		mVisible = false;
	}
}

void CubemapEntity::RenderImguiSelf()
{
	mCubemapTexture->RenderImgui(256, 256);
}