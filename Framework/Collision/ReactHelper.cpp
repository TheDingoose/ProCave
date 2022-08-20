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
	//return XMVectorSet(
	//transform.getOrientation().getVectorV().x,
	//transform.getOrientation().getVectorV().y,
	//transform.getOrientation().getVectorV().z, 0.f);
	//
	//XmmatrixRotationXMMatrixRotationQuaternion()
	
	//float heading;
	//float attitude;
	//float bank;
	//
	//float t = Quat.m128_f32[0] * Quat.m128_f32[1] + Quat.m128_f32[2] * Quat.m128_f32[3];
	//if (t > 0.4999f) {
	//	heading = 2 * atan2(Quat.m128_f32[0], Quat.m128_f32[3]);
	//	attitude = PI / 2;
	//	bank = 0;
	//}
	//else if (t < -0.4999) {
	//	heading = -2 * atan2(Quat.m128_f32[0], Quat.m128_f32[3]);
	//	attitude = PI / 2;
	//	bank = 0;
	//}
	//else {
	//	float sqx = pow(Quat.m128_f32[0], 2);
	//	float sqy = pow(Quat.m128_f32[1], 2);
	//	float sqz = pow(Quat.m128_f32[2], 2);
	//	heading = atan2(2 * Quat.m128_f32[1] * Quat.m128_f32[3] - 2 * Quat.m128_f32[1] * Quat.m128_f32[2], 
	//	1 - 2 * sqy - 2 * sqz);
	//	attitude = asin(2 * t);
	//	bank = atan2(2 * Quat.m128_f32[0] * Quat.m128_f32[3] - 2 * Quat.m128_f32[1] * Quat.m128_f32[2], 
	//	1 - 2 * sqx - 2 * sqz);
	//}
	//return XMVectorSet(attitude, heading, bank,  0.f);
	
	
	const auto x = Quat.m128_f32[1];
	const auto y = Quat.m128_f32[2];
	const auto z = Quat.m128_f32[0];
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





//	XMVECTOR retVector;
//
//	const auto x = Quat.m128_f32[1];
//	const auto y = Quat.m128_f32[2];
//	const auto z = Quat.m128_f32[0];
//	const auto w = Quat.m128_f32[3];
//
//	retVector.m128_f32[2] = atan2(2.0 * (y * z + w * x), w * w - x * x - y * y + z * z);
//	retVector.m128_f32[1] = asin(-2.0 * (x * z - w * y));
//	retVector.m128_f32[0] = atan2(2.0 * (x * y + w * z), w * w + x * x - y * y - z * z);
//
//#if 0
//	retVector.m128_f32[0] = (retVector.m128_f32[0] * (180 / PI));
//	retVector.m128_f32[1] = (retVector.m128_f32[1] * (180 / PI)) * -1;
//	retVector.m128_f32[2] = retVector.m128_f32[2] * (180 / PI);
//#endif
//	return retVector;

}


