#pragma once
#include "core/entities/entity.h"

class CameraEntity : public Entity
{
public:
	CameraEntity(const std::string& name);
	~CameraEntity();

protected:
	virtual void UpdateSelf(float delta) override;
	virtual void RenderSelf(RenderServer& renderServer) override;
	virtual void RenderImguiSelf() override;
};