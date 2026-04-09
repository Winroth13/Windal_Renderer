#pragma once
#include "core/transform.h"
#include "core/logger.h"

using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	Transform transform;

	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	void SetLens(float fovY, float aspect, float zNear, float zFar);

	DirectX::XMMATRIX GetView() const;
	DirectX::XMMATRIX GetProj() const;
	DirectX::XMMATRIX GetViewProj() const;

	DirectX::XMFLOAT4X4 GetView4x4f() const;
	DirectX::XMFLOAT4X4 GetProj4x4f() const;

	void UpdateViewMatrix()
	{
		XMFLOAT3 right = transform.GetRightDir3f();
		XMFLOAT3 up = transform.GetUpDir3f();
		XMFLOAT3 look = transform.GetForwardDir3f();
		XMFLOAT3 pos = transform.GetPosition3f();

		//Logger::Info("Right Vector: (" + std::to_string(right.x) + ", " + std::to_string(right.y) + ", " + std::to_string(right.z) + ")");
		//Logger::Info("Forward Vector: (" + std::to_string(look.x) + ", " + std::to_string(look.y) + ", " + std::to_string(look.z) + ")");
		//Logger::Info("Up Vector: (" + std::to_string(up.x) + ", " + std::to_string(up.y) + ", " + std::to_string(up.z) + ")");

		XMVECTOR R = XMLoadFloat3(&right);
		XMVECTOR U = XMLoadFloat3(&up);
		XMVECTOR L = XMLoadFloat3(&look);
		XMVECTOR P = XMLoadFloat3(&pos);

		L = XMVector3Normalize(L);
		U = XMVector3Normalize(XMVector3Cross(L, R));

		R = XMVector3Cross(U, L);

		float x = -XMVectorGetX(XMVector3Dot(P, R));
		float y = -XMVectorGetX(XMVector3Dot(P, U));
		float z = -XMVectorGetX(XMVector3Dot(P, L));

		XMStoreFloat3(&right, R);
		XMStoreFloat3(&up, U);
		XMStoreFloat3(&look, L);

		mView(0, 0) = right.x;
		mView(1, 0) = right.y;
		mView(2, 0) = right.z;
		mView(3, 0) = x;

		mView(0, 1) = up.x;
		mView(1, 1) = up.y;
		mView(2, 1) = up.z;
		mView(3, 1) = y;

		mView(0, 2) = look.x;
		mView(1, 2) = look.y;
		mView(2, 2) = look.z;
		mView(3, 2) = z;

		mView(0, 3) = 0.0f;
		mView(1, 3) = 0.0f;
		mView(2, 3) = 0.0f;
		mView(3, 3) = 1.0f;
	}

private:
	float mNearZ = 0.0f;
	float mFarZ = 0.0f;
	float mAspect = 0.0f;
	float mFovY = 0.0f;
	float mNearWindowHeight = 0.0f;
	float mFarWindowHeight = 0.0f;

	bool mViewDirty = true;

	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;
};