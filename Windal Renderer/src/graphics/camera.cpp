#include "graphics/camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
	constexpr float fov = 1.396263f; // 80 degrees in radians
	SetLens(fov, 16.f / 9.f, 0.1f, 1000.0f); // give the camera some sensible defaults
}

Camera::~Camera()
{

}

float Camera::GetNearZ() const
{
	return mNearZ;
}

float Camera::GetFarZ() const
{
	return mFarZ;
}

float Camera::GetAspect() const
{
	return mAspect;
}

float Camera::GetFovY() const
{
	return mFovY;
}

float Camera::GetFovX() const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atanf(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth() const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return mFarWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zNear, float zFar)
{
	mFovY = fovY;
	mAspect = aspect;
	mNearZ = zNear;
	mFarZ = zFar;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

	DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

DirectX::XMMATRIX Camera::GetView() const
{
	return XMLoadFloat4x4(&mView);
}

DirectX::XMMATRIX Camera::GetProj() const
{
	return XMLoadFloat4x4(&mProj);
}

DirectX::XMMATRIX Camera::GetViewProj() const
{
	return GetView() * GetProj();
}

DirectX::XMFLOAT4X4 Camera::GetView4x4f() const
{
	return mView;
}

DirectX::XMFLOAT4X4 Camera::GetProj4x4f() const
{
	return mProj;
}