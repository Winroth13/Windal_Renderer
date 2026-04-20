#include "core/entities/cameraentity.h"
#include "core/scene.h"

#include "imgui/imgui.h"
#include "core/imguiflags.h"

CameraEntity::CameraEntity(const std::string& name)
	: Entity(name)
{	
}

CameraEntity::~CameraEntity() {}

void CameraEntity::UpdateSelf(double delta)
{
	GetScene().GetCamera().transform.SetPosition(transform.GetPosition3f());
	GetScene().GetCamera().transform.SetAngles(transform.GetAngles3f());
	GetScene().GetCamera().transform.SetScale(transform.GetScale3f());
}

void CameraEntity::RenderSelf(RenderServer& renderServer)
{
}

void CameraEntity::RenderImguiSelf()
{
	if (ImGui::TreeNodeEx("Camera Properties", TREE_NODE_FLAGS))
	{
		float fovInDegrees = GetScene().GetCamera().GetFovY() * (180.0f / 3.14159f);
		if (ImGui::DragFloat("Fov", &fovInDegrees, 1, 1, 180, "%.f deg"))
		{
			auto& camera = GetScene().GetCamera();
			camera.SetPerspectiveLens(
				fovInDegrees * (3.14159f / 180),
				camera.GetAspect(),
				camera.GetNearZ(),
				camera.GetFarZ()
			);
		}
		ImGui::TreePop();
	}
}