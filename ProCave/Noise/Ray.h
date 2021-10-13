#pragma once

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

struct NoiseCollisionShape {
	std::vector<XMVECTOR> Positions;
};


class NoiseRay
{
public:
	static XMVECTOR Test(XMVECTOR Pos, XMVECTOR Direction);
	static XMVECTOR GetNormal(XMVECTOR Pos);
	static XMVECTOR GetNormal(XMVECTOR Pos, float Offset);
	static bool HitCheck(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR CollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR InvertedCollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static XMVECTOR DensityCollisionTest(XMVECTOR Pos, XMVECTOR Velocity);
	static void Decollide(XMVECTOR* Pos, NoiseCollisionShape Shape);
	static void Decollide2(XMVECTOR* Pos, NoiseCollisionShape Shape);
	static void DensityCollisionVelocityTest(XMVECTOR* Pos, XMVECTOR* Velocity, float DeltaTime);
	static void DensityCollisionVelocityTest(XMVECTOR* Pos, NoiseCollisionShape Shape, XMVECTOR* Velocity, float DeltaTime);
	static void LoosyCollisionTest(XMVECTOR* Pos, NoiseCollisionShape Shape, XMVECTOR* Velocity, float DeltaTime);
	

};

