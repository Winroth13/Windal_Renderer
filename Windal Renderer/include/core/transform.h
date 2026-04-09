#pragma once
#include <DirectXMath.h>

class Transform
{
public:
	Transform() {};
	~Transform() {};

	// Get/Set tranmsform world position
	DirectX::XMVECTOR GetPosition() const;
	DirectX::XMFLOAT3 GetPosition3f();
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& position);

	// Get/Set transform world angles
	DirectX::XMVECTOR GetAngles() const;
	DirectX::XMFLOAT3 GetAngles3f() const;
	void SetAngles(float pitch, float yaw, float roll);
	void SetAngles(const DirectX::XMFLOAT3& angles);

	// Get/Set transform world scale
	DirectX::XMVECTOR GetScale() const;
	DirectX::XMFLOAT3 GetScale3f() const;
	void SetScale(float x, float y, float z);
	void SetScale(const DirectX::XMFLOAT3& scale);

	// Get transform basis vectors
	DirectX::XMVECTOR GetRightDir() const;
	DirectX::XMFLOAT3 GetRightDir3f();
	DirectX::XMVECTOR GetUpDir() const;
	DirectX::XMFLOAT3 GetUpDir3f();
	DirectX::XMVECTOR GetForwardDir() const;
	DirectX::XMFLOAT3 GetForwardDir3f();

	// Define transform space via LookAt parameters
	void LookAt(
		DirectX::FXMVECTOR position,
		DirectX::FXMVECTOR target,
		DirectX::FXMVECTOR up
	);
	void LookAt(
		const DirectX::XMFLOAT3& position,
		const DirectX::XMFLOAT3& target,
		const DirectX::XMFLOAT3& up
	);

	void LookAt(DirectX::FXMVECTOR target);
	void LookAt(const DirectX::XMFLOAT3& target);

	void LookAt(const Transform& target);

	// Get matrix
	const DirectX::XMMATRIX& GetMatrix();
	const DirectX::XMFLOAT4X4& GetMatrixf();

	// Move in global space
	void MoveX(float distance);
	void MoveY(float distance);
	void MoveZ(float distance);

	// Move in local space
	void MoveRight(float distance);
	void MoveUp(float distance);
	void MoveForward(float distance);

	// Rotate in global space
	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	// Rotate in local space
	void RotatePitch(float angle);
	void RotateYaw(float angle);
	void RotateRoll(float angle);

private:
	DirectX::XMFLOAT3 mPosition = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 mAngles = { 0, 0, 0 };
	DirectX::XMFLOAT3 mScale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4 mMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	bool mIsDirty = false;

	DirectX::XMFLOAT3 AnglesFromMatrix(DirectX::XMFLOAT4X4& matrix);
};