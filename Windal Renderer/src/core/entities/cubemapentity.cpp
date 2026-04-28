#include "core/entities/cubemapentity.h"
#include "graphics/textures/cubemaptexture.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

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
	if (mIsDynamic || mShouldCapture)
	{
		renderServer.PushCubemap(GetGlobalPosition(), mCubemapTexture);
		mShouldCapture = false;
	}
}

void CubemapEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Cubemap", TREE_NODE_FLAGS))
	{
		ImGui::Checkbox("Dynamic", &mIsDynamic);

		if (!mIsDynamic)
		{
			if (ImGui::Button("Capture"))
			{
				mShouldCapture = true;
			}
		}
		ImGui::TreePop();
	}
}