#pragma once
#include "core/entities/entity.h"

class Model;

class ModelEntity : public Entity
{
public:
	ModelEntity(std::shared_ptr<Model> model);
	~ModelEntity();

	bool IsStatic() const { return mStatic; }
	void SetStatic(bool value) { mStatic = value; }

protected:
	virtual void BeginSelf(RenderServer& renderServer) override;
	virtual void UpdateSelf(double delta) override;
	virtual void RenderSelf(RenderServer& renderServer) override;
	virtual void RenderImguiSelf() override;

private:
	std::shared_ptr<Model> mModel;
	bool mStatic = false;
};