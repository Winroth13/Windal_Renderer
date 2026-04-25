#include "core/entities/pointlightentity.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

PointLightEntity::PointLightEntity() : Entity("Point Light") {}

PointLightEntity::~PointLightEntity() {}

void PointLightEntity::UpdateSelf(double delta)
{
}

void PointLightEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.PushPointLight(transform.GetPosition3f(), mColor, mAttenuation, mIntensity);
}

void PointLightEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Light Properties", TREE_NODE_FLAGS))
	{
		ImGui::ColorEdit3("Color", &mColor.x);
		ImGui::DragFloat("Attenuation", &mAttenuation, 0.02f, 0.01f, 20.0f);
		ImGui::DragFloat("Intensity", &mIntensity);
		ImGui::TreePop();
	}
}