#include "core/entities/modelentity.h"
#include "graphics/models/model.h"
#include "core/renderserver.h"
#include "core/logger.h"

#include <d3d11.h>

ModelEntity::ModelEntity(const std::string& name, std::shared_ptr<Model> model)
	: Entity(name), mModel(model)
{
}

ModelEntity::~ModelEntity() {}

void ModelEntity::UpdateSelf(float delta)
{
}

void ModelEntity::RenderSelf(RenderServer& renderServer)
{
	Logger::Info("Rendering Model Entity");
	mModel->Bind(renderServer);
	renderServer.RenderIndexed(mModel->GetNumIndicies());
}

void ModelEntity::RenderImguiSelf()
{
	mModel->RenderImgui();
}