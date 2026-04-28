#include "core/entities/modelentity.h"
#include "graphics/models/model.h"
#include "graphics/meshes/mesh.h"
#include "core/renderer/renderserver.h"
#include "core/logger.h"

#include <d3d11.h>

ModelEntity::ModelEntity(std::shared_ptr<Model> model)
	: mModel(model), Entity("Model")
{
}

ModelEntity::~ModelEntity() {}

void ModelEntity::UpdateSelf(double delta)
{
	//transform.RotateY((3.14f / 8) * static_cast<float>(delta));
	//transform.RotateX((3.14f / 8) * static_cast<float>(delta));
	//transform.RotateZ((3.14f / 8) * static_cast<float>(delta));
}

#include "math/mathfunctions.h"

void ModelEntity::RenderSelf(RenderServer& renderServer)
{
	for (size_t meshIndex = 0; meshIndex < mModel->GetMeshCount(); ++meshIndex)
	{
		if (mModel->IsMeshVisible(meshIndex))
		{
			renderServer.PushMesh(mModel->GetMesh(meshIndex), GetGlobalTransform());
			renderServer.PushMaterial(mModel->GetMaterial(meshIndex));

			for (auto& mesh : mModel->GetMeshes())
			{
				AABB localBounds = mesh->GetBounds();
				AABB bounds = localBounds.Transform(GetGlobalTransform());
				renderServer.PushAABB(bounds, { 0, 1.0f, 0 });
			}
		}
	}
}

void ModelEntity::RenderImguiSelf()
{
	mModel->RenderImgui();
}