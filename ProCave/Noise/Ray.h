#pragma once

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

struct CollisionShape {
	std::vector<XMVECTOR> Positions;
};


class Ray
{
public:
	static XMVECTOR Test(XMVECTOR Pos, XMVECTOR Direction);
	static XMVECTOR GetNormal(XMVECTOR Pos);
	static XMVECTOR GetNormal(XMVECTOR Pos, float Offset);
	static bool HitCheck(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR CollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR InvertedCollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR DensityCollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static void Decollide(XMVECTOR* Pos, CollisionShape Shape);
	static void Decollide2(XMVECTOR* Pos, CollisionShape Shape);
	static void DensityCollisionVelocityTest(XMVECTOR* Pos, XMVECTOR* Velocity, float DeltaTime);
	static void DensityCollisionVelocityTest(XMVECTOR* Pos, CollisionShape Shape, XMVECTOR* Velocity, float DeltaTime);
	static void LoosyCollisionTest(XMVECTOR* Pos, CollisionShape Shape, XMVECTOR* Velocity, float DeltaTime);
	

};

