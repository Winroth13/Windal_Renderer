#include "core/transform.h"

using namespace DirectX;

/* Get/Set Position */
XMVECTOR Transform::GetPosition() const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Transform::GetPosition3f() const
{
	return mPosition;
}

void Transform::SetPosition(float x, float y, float z)
{
	mPosition = DirectX::XMFLOAT3(x, y, z);
	mIsDirty = true;
}

void Transform::SetPosition(const XMFLOAT3& position)
{
	mPosition = position;
	mIsDirty = true;
}

/* Get/Set Angles */
XMVECTOR Transform::GetAngles() const
{
	return XMLoadFloat3(&mAngles);
}

XMFLOAT3 Transform::GetAngles3f() const
{
	return mAngles;
}

void Transform::SetAngles(float pitch, float yaw, float roll)
{
	mAngles = XMFLOAT3(pitch, yaw, roll);
	mIsDirty = true;
}

void Transform::SetAngles(const XMFLOAT3& angles)
{
	mAngles = angles;
	mIsDirty = true;
}

/* Get/Set Scale */
XMVECTOR Transform::GetScale() const
{
	return XMLoadFloat3(&mScale);
}

XMFLOAT3 Transform::GetScale3f() const
{
	return mScale;
}

void Transform::SetScale(float x, float y, float z)
{
	mScale = XMFLOAT3(x, y, z);
	mIsDirty = true;
}

void Transform::SetScale(const XMFLOAT3& scale)
{
	mScale = scale;
	mIsDirty = true;
}

/* Get Directions */
XMVECTOR Transform::GetRightDir() const
{
	XMVECTOR right = { 1, 0, 0 };
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mAngles));
	right = XMVector3Transform(right, rotationMatrix);
	return right;
}

XMFLOAT3 Transform::GetRightDir3f() const
{
	XMFLOAT3 right;
	XMStoreFloat3(&right, GetRightDir());
	return right;
}

XMVECTOR Transform::GetUpDir() const
{
	XMVECTOR up = { 0, 1, 0 };
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mAngles));
	up = XMVector3Transform(up, rotationMatrix);
	return up;
}

XMFLOAT3 Transform::GetUpDir3f() const
{
	XMFLOAT3 up;
	XMStoreFloat3(&up, GetUpDir());
	return up;
}

XMVECTOR Transform::GetForwardDir() const
{
	XMVECTOR forward = { 0, 0, 1 };
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mAngles));
	forward = XMVector3Transform(forward, rotationMatrix);
	return forward;
}

XMFLOAT3 Transform::GetForwardDir3f() const
{
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, GetForwardDir());
	return forward;
}

/* Define transform space via LookAt parameters */
void Transform::LookAt(
	DirectX::FXMVECTOR position,
	DirectX::FXMVECTOR target,
	DirectX::FXMVECTOR up
)
{
	XMStoreFloat4x4(
		&mMatrix,
		XMMatrixLookAtLH(
			position,
			target,
			up
		)
	);

	mAngles = AnglesFromMatrix(mMatrix);

	mIsDirty = false;
}

void Transform::LookAt(
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT3& target,
	const DirectX::XMFLOAT3& up
)
{
	LookAt(
		XMLoadFloat3(&position),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up)
	);
}

void Transform::LookAt(DirectX::FXMVECTOR target)
{
	LookAt(GetPosition(), target, GetUpDir());
}

void Transform::LookAt(const DirectX::XMFLOAT3& target)
{
	LookAt(GetPosition(), XMLoadFloat3(&target), GetUpDir());
}

void Transform::LookAt(const Transform& target)
{
	LookAt(GetPosition(), target.GetPosition(), GetUpDir());
}

// Get matrix
DirectX::XMMATRIX Transform::GetMatrix()
{
	if (mIsDirty)
	{
		XMMATRIX newMatrix = DirectX::XMMatrixIdentity();
		newMatrix *= DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
		newMatrix *= XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mAngles));
		newMatrix *= DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		XMStoreFloat4x4(&mMatrix, newMatrix);
	}

	return XMLoadFloat4x4(&mMatrix);
}

DirectX::XMFLOAT4X4 Transform::GetMatrixf()
{
	if (mIsDirty)
	{
		XMMATRIX newMatrix = DirectX::XMMatrixIdentity();
		newMatrix *= DirectX::XMMatrixScaling(mScale.x, mScale.y, mScale.z);
		newMatrix *= XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mAngles));
		newMatrix *= DirectX::XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
		XMStoreFloat4x4(&mMatrix, newMatrix);
	}

	return mMatrix;
}

// Move in global space
void Transform::MoveX(float distance)
{
	mPosition.x += distance,
		mIsDirty = true;
}

void Transform::MoveY(float distance)
{
	mPosition.y += distance,
		mIsDirty = true;
}

void Transform::MoveZ(float distance)
{
	mPosition.z += distance,
		mIsDirty = true;
}

// Move in local space
void Transform::MoveRight(float distance)
{
	XMVECTOR vecDistance = XMVectorReplicate(distance);
	XMVECTOR vecRight = GetRightDir();
	XMVECTOR vecPosition = GetPosition();
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vecDistance, vecRight, vecPosition));

	mIsDirty = true;
}

void Transform::MoveUp(float distance)
{
	XMVECTOR vecDistance = XMVectorReplicate(distance);
	XMVECTOR vecUp = GetUpDir();
	XMVECTOR vecPosition = GetPosition();
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vecDistance, vecUp, vecPosition));

	mIsDirty = true;
}

void Transform::MoveForward(float distance)
{
	XMVECTOR vecDistance = XMVectorReplicate(distance);
	XMVECTOR vecForward = GetForwardDir();
	XMVECTOR vecPosition = GetPosition();
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vecDistance, vecForward, vecPosition));

	mIsDirty = true;
}

// Rotate in global space
void Transform::RotateX(float angle)
{
	mAngles.x += angle;
	mIsDirty = true;
}

void Transform::RotateY(float angle)
{
	mAngles.y += angle;
	mIsDirty = true;
}

void Transform::RotateZ(float angle)
{
	mAngles.z += angle;
	mIsDirty = true;
}

// Rotate in local space
void Transform::RotatePitch(float angle)
{
	// Rotate up and look vector about the right vector
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(GetRightDir(), angle);
	XMFLOAT4X4 rotationMatrixf;
	XMStoreFloat4x4(&rotationMatrixf, rotationMatrix);
	XMFLOAT3 angles = AnglesFromMatrix(rotationMatrixf);
	mAngles.x += angles.x;
	mAngles.y += angles.y;
	mAngles.z += angles.z;

	mIsDirty = true;
}

void Transform::RotateYaw(float angle)
{
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(GetUpDir(), angle);
	XMFLOAT4X4 rotationMatrixf;
	XMStoreFloat4x4(&rotationMatrixf, rotationMatrix);
	XMFLOAT3 angles = AnglesFromMatrix(rotationMatrixf);
	mAngles.x += angles.x;
	mAngles.y += angles.y;
	mAngles.z += angles.z;

	mIsDirty = true;
}

void Transform::RotateRoll(float angle)
{
	XMMATRIX rotationMatrix = XMMatrixRotationAxis(GetForwardDir(), angle);
	XMFLOAT4X4 rotationMatrixf;
	XMStoreFloat4x4(&rotationMatrixf, rotationMatrix);
	XMFLOAT3 angles = AnglesFromMatrix(rotationMatrixf);
	mAngles.x += angles.x;
	mAngles.y += angles.y;
	mAngles.z += angles.z;

	mIsDirty = true;
}

XMFLOAT3 Transform::AnglesFromMatrix(XMFLOAT4X4& matrix)
{
	float pitch = (float)atan2(-matrix._31, sqrt(matrix._32 * matrix._32 + matrix._33 * matrix._33));
	float yaw = (float)atan2(matrix._21, matrix._11);
	float roll = (float)atan2(matrix._32, matrix._33);
	return XMFLOAT3(pitch, yaw, roll);
}