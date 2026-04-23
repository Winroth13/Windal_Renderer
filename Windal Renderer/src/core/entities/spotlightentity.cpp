#include "core/entities/spotlightentity.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

SpotLightEntity::SpotLightEntity(const std::string& name)
	:Entity(name)
{
}

SpotLightEntity::~SpotLightEntity()
{
}

void SpotLightEntity::UpdateSelf(double delta)
{
}

void SpotLightEntity::RenderSelf(RenderServer& renderServer)
{
	renderServer.PushSpotLight(
		transform.GetForwardDir3f(),
		transform.GetPosition3f(),
		mColor,
		mAngle,
		mIntensity,
		mAttenuation
	);
}

void SpotLightEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Light Properties", TREE_NODE_FLAGS))
	{
		ImGui::ColorEdit3("Color", &mColor.x);
		ImGui::DragFloat("Intensity", &mIntensity);
		ImGui::DragFloat("Attenuation", &mAttenuation, 0.02f, 0.01f, 20.0f);
		ImGui::DragFloat("Angle", &mAngle, 0.02f, 0, DirectX::XM_PI);

		DirectX::XMFLOAT3 dir = transform.GetForwardDir3f();
		ImGui::Text("Direction: (%.3f, %.3f, %.3f)", dir.x, dir.y, dir.z);
		ImGui::TreePop();
	}
}