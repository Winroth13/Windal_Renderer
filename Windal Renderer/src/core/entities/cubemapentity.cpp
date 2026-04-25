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
}

void CubemapEntity::RenderImguiSelf()
{
}