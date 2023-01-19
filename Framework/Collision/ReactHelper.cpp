#include "ReactHelper.h"

#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <vector>

#include "reactphysics3d/reactphysics3d.h"

using namespace reactphysics3d;
using namespace DirectX;


DirectX::XMVECTOR GetRePhyTranslation(reactphysics3d::Transform transform)
{
	return XMVectorSet(
		transform.getPosition().x,
		transform.getPosition().y,
		transform.getPosition().z,
		0.f);
}

DirectX::XMVECTOR GetRePhyRotationQuat(reactphysics3d::Transform transform)
{
	return XMVectorSet(
		transform.getOrientation().x,
		transform.getOrientation().y,
		transform.getOrientation().z,
		transform.getOrientation().w);
}

DirectX::XMVECTOR GetRePhyRotation(reactphysics3d::Transform transform)
{ 
	XMVECTOR Quat = GetRePhyRotationQuat(transform);
	
	float heading;
	float attitude;
	float bank;

	const auto x = Quat.m128_f32[0];
	const auto y = Quat.m128_f32[1];
	const auto z = Quat.m128_f32[2];
	const auto w = Quat.m128_f32[3];


	float xx = x * x;
	float yy = y * y;
	float zz = z * z;

	float m31 = 2.f * x * z + 2.f * y * w;
	float m32 = 2.f * y * z - 2.f * x * w;
	float m33 = 1.f - 2.f * xx - 2.f * yy;

	float cy = sqrtf(m33 * m33 + m31 * m31);
	float cx = atan2f(-m32, cy);
	if (cy > 16.f * FLT_EPSILON)
	{
		float m12 = 2.f * x * y + 2.f * z * w;
		float m22 = 1.f - 2.f * xx - 2.f * zz;

		return XMVectorSet(cx, atan2f(m31, m33), atan2f(m12, m22), 0.f);
	}
	else
	{
		float m11 = 1.f - 2.f * yy - 2.f * zz;
		float m21 = 2.f * x * y - 2.f * z * w;

		return XMVectorSet(cx, 0.f, atan2f(-m21, m11), 0.f);
	}
}


