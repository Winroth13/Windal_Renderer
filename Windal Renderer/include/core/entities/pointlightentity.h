#pragma once
#include "core/entities/entity.h"

class PointLightEntity : public Entity
{
public:
	PointLightEntity(const std::string& name);
	~PointLightEntity();

protected:
	virtual void UpdateSelf(double delta) override;
	virtual void RenderSelf(RenderServer& renderServer) override;
	virtual void RenderImguiSelf() override;

private:
	DirectX::XMFLOAT3 mColor = { 1.0, 1.0, 1.0 };
	float mRange = 2.0f;
	float mIntensity = 1.0f;
};
