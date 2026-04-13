#include "core/entities/pointlightentity.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

PointLightEntity::PointLightEntity(const std::string& name)
	:Entity(name)
{
}

PointLightEntity::~PointLightEntity()
{
}

void PointLightEntity::UpdateSelf(double delta)
{
}

void PointLightEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.PushPointLight(transform.GetPosition3f(), mColor, mRange, mIntensity);
}

void PointLightEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Point Light Properties", TREE_NODE_FLAGS))
	{
		ImGui::ColorEdit3("Color", &mColor.x);
		ImGui::DragFloat("Range", &mRange, 1.0f, 0.01f);
		ImGui::DragFloat("Intensity", &mIntensity);
		ImGui::TreePop();
	}
}
