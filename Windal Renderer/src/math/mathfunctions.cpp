#include "math/mathfunctions.h"

DirectX::XMFLOAT3 DirectionToAngles(DirectX::XMFLOAT3 direction)
{
	return DirectX::XMFLOAT3(
		atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z)),
		atan2f(direction.x, direction.z),
		0
	);
}