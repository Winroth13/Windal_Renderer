#include "core/entities/directionallightentity.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

DirectionalLightEntity::DirectionalLightEntity(const std::string& name)
	:Entity(name)
{
}

DirectionalLightEntity::~DirectionalLightEntity()
{
}

void DirectionalLightEntity::UpdateSelf(double delta)
{
}

void DirectionalLightEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.PushDirectionalLight(mDirection, mColor, mIntensity);
}

void DirectionalLightEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Point Light Properties", TREE_NODE_FLAGS))
	{
		ImGui::DragFloat3("Direction", &mDirection.x, 0.01f);
		ImGui::ColorEdit3("Color", &mColor.x);
		ImGui::DragFloat("Intensity", &mIntensity);
		ImGui::TreePop();
	}
}
