#pragma once
#include "core/entities/entity.h"

class Model;

class ModelEntity : public Entity
{
public:
	ModelEntity(
		const std::string& name,
		std::shared_ptr<Model> model
	);
	~ModelEntity();

protected:
	virtual void UpdateSelf(double delta) override;
	virtual void RenderSelf(RenderServer& renderServer) override;
	virtual void RenderImguiSelf() override;

private:
	std::shared_ptr<Model> mModel;
};