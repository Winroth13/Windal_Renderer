#include "core/entities/modelentity.h"
#include "graphics/models/model.h"
#include "core/renderserver.h"
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

void ModelEntity::RenderSelf(RenderServer& renderServer)
{
	for (size_t meshIndex = 0; meshIndex < mModel->GetMeshCount(); ++meshIndex)
	{
		if (mModel->IsMeshVisible(meshIndex))
		{
			renderServer.PushMesh(mModel->GetMesh(meshIndex), transform.GetMatrix());
			renderServer.PushMaterial(mModel->GetMaterial(meshIndex));
		}
	}
}

void ModelEntity::RenderImguiSelf()
{
	mModel->RenderImgui();
}