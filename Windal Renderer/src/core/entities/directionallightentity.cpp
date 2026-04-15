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
	renderServer.PushDirectionalLight(transform.GetForwardDir3f(), mColor, mIntensity);
}

void DirectionalLightEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Point Light Properties", TREE_NODE_FLAGS))
	{
		ImGui::ColorEdit3("Color", &mColor.x);
		ImGui::DragFloat("Intensity", &mIntensity);

		DirectX::XMFLOAT3 dir = transform.GetForwardDir3f();
		ImGui::Text("Direction: (%.3f, %.3f, %.3f)", dir.x, dir.y, dir.z);
		ImGui::TreePop();
	}
}
