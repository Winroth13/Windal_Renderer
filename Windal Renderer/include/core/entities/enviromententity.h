#pragma once
#include "core/entities/entity.h"


class EnviromentEntity : public Entity
{
public:
	EnviromentEntity(const std::string& name);
	~EnviromentEntity();

protected:
	virtual void UpdateSelf(double delta) override;
	virtual void RenderSelf(RenderServer& renderServer) override;
	virtual void RenderImguiSelf() override;
};