#pragma once
#include "core/entities/entity.h"

class DirectionalLightEntity : public Entity
{
public:
	DirectionalLightEntity(const std::string& name);
	~DirectionalLightEntity();

	void SetColor(const DirectX::XMFLOAT3 color) { mColor = color; };
	void SetDirection(const DirectX::XMFLOAT3 direction) { mDirection = direction; };
	void SetIntensity(const float intensity) { mIntensity = intensity; };

	DirectX::XMFLOAT3 GetColor() const { return mColor; };
	DirectX::XMFLOAT3 GetDirection() const { return mDirection; };
	float GetIntensity() const { return mIntensity; };

protected:
	virtual void UpdateSelf(double delta) override;
	virtual void RenderSelf(RenderServer& renderServer) override;
	virtual void RenderImguiSelf() override;

private:
	DirectX::XMFLOAT3 mColor = { 1.0, 1.0, 1.0 };
	DirectX::XMFLOAT3 mDirection = { 0.0f, -1.0f, 0.0f };
	float mIntensity = 1.0f;
};
