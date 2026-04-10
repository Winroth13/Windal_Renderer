#include "graphics/enviroment.h"

Enviroment::Enviroment()
{
}

Enviroment::~Enviroment()
{
}

void Enviroment::SetAmbientColor(float r, float g, float b)
{
	mAmbientColor.x = r;
	mAmbientColor.y = g;
	mAmbientColor.z = b;
}

void Enviroment::SetSunColor(float r, float g, float b)
{
	mSunColor.x = r;
	mSunColor.y = r;
	mSunColor.z = b;
}

void Enviroment::SetSunDirection(float r, float g, float b)
{
	mSunDirection.x = r;
	mSunDirection.y = g;
	mSunDirection.z = b;
}

const DirectX::XMFLOAT3 Enviroment::GetAmbientColor()
{
	return mAmbientColor;
}

const DirectX::XMFLOAT3 Enviroment::GetSunColor()
{
	return mSunColor;
}

const DirectX::XMFLOAT3 Enviroment::GetSunDirection()
{
	return mSunDirection;
}

DirectX::XMFLOAT3& Enviroment::GetAmbientColorRef()
{
	return mAmbientColor;
}

DirectX::XMFLOAT3& Enviroment::GetSunColorRef()
{
	return mSunColor;
}

DirectX::XMFLOAT3& Enviroment::GetSunDirectionRef()
{
	return mSunDirection;
}
