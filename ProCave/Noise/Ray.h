#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Ray
{
public:
	static XMVECTOR Test(XMVECTOR Pos, XMVECTOR Direction);
	static XMVECTOR GetNormal(XMVECTOR Pos);
	static bool HitCheck(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR CollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR InvertedCollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR DensityCollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static void DensityCollisionVelocityTest(XMVECTOR* Pos, XMVECTOR* Velocity, float DeltaTime);
	

};

