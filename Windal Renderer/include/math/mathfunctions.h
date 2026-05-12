#pragma once
#include <DirectXMath.h>
#include <string>

DirectX::XMFLOAT3 DirectionToAngles(DirectX::XMFLOAT3 direction);

std::string Float3ToString(const DirectX::XMFLOAT3& float3);

std::string VectorToString(const DirectX::XMVECTOR float3);

void PreventGimbalLock(DirectX::XMFLOAT3& rotation);

DirectX::XMFLOAT3 EulerFromQuaternion(const DirectX::XMFLOAT4& quaternion);

DirectX::XMFLOAT3 AnglesFromMatrix(DirectX::XMMATRIX matrix);