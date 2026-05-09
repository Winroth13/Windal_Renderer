#include "core/entities/modelentity.h"
#include "graphics/models/model.h"
#include "graphics/meshes/mesh.h"

#include "core/renderer/renderserver.h"
#include "core/logger.h"

#include "math/mathfunctions.h"

#include "imgui/imgui.h"

#include <d3d11.h>

ModelEntity::ModelEntity(std::shared_ptr<Model> model)
	: mModel(model), Entity("Model")
{
}

ModelEntity::~ModelEntity() {}

void ModelEntity::BeginSelf(RenderServer& renderServer)
{
	/* Push static geometry on scene begin */
	if (mStatic)
	{
		for (size_t meshIndex = 0; meshIndex < mModel->GetMeshCount(); ++meshIndex)
		{
			renderServer.PushMesh(mModel->GetMesh(meshIndex), GetGlobalTransform(), true);
			renderServer.PushMaterial(mModel->GetMaterial(meshIndex), true);
		}
	}
}

void ModelEntity::UpdateSelf(double delta)
{
	//transform.RotateY((3.14f / 8) * static_cast<float>(delta));
	//transform.RotateX((3.14f / 8) * static_cast<float>(delta));
	//transform.RotateZ((3.14f / 8) * static_cast<float>(delta));
}

void ModelEntity::RenderSelf(RenderServer& renderServer)
{
	if (!mStatic)
	{
		for (size_t meshIndex = 0; meshIndex < mModel->GetMeshCount(); ++meshIndex)
		{
			if (mModel->IsMeshVisible(meshIndex))
			{
				renderServer.PushMesh(mModel->GetMesh(meshIndex), GetGlobalTransform());
				renderServer.PushMaterial(mModel->GetMaterial(meshIndex));
			}
		}
	}
}

void ModelEntity::RenderImguiSelf()
{
	if (mStatic)
	{
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "This model is static! No changes here will take effect!");
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 1.0f, 0, 0.25f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 1.0f, 0, 0.25f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 1.0f, 0, 0.25f));
	}

	mModel->RenderImgui();

	if (mStatic)
		ImGui::PopStyleColor(3);
}