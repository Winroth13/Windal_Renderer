#pragma once

#include <array>
#include <DirectXMath.h>

class Enviroment
{
public:
	Enviroment();
	~Enviroment();

	void SetAmbientColor(float r, float g, float b);
	void SetSunColor(float r, float g, float b);
	void SetSunDirection(float r, float g, float b);

	const DirectX::XMFLOAT3 GetAmbientColor();
	const DirectX::XMFLOAT3 GetSunColor();
	const DirectX::XMFLOAT3 GetSunDirection();

	DirectX::XMFLOAT3& GetAmbientColorRef();
	DirectX::XMFLOAT3& GetSunColorRef();
	DirectX::XMFLOAT3& GetSunDirectionRef();

private:
	DirectX::XMFLOAT3 mAmbientColor = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 mSunColor = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 mSunDirection = { 1.0f, 0.0f, 0.0f };
};