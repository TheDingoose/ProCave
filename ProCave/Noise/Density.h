#pragma once
#include <vector>
#include <DirectXMath.h>
#include "perlin/PerlinNoise.hpp"

using namespace DirectX;

static siv::PerlinNoise perlin;

static float GetDensity(XMFLOAT4 aPos) {
	float Density = 0.f;

	//Density -= aPos.y;
	
	aPos.x *= 0.05;
	aPos.y *= 0.05;
	aPos.z *= 0.05;



	//Density += (fmod(((float)rand()) , 100.f) / 100.f) / aPos.y;
	Density += std::fmin(perlin.noise3D(aPos.x, aPos.y, aPos.z) * 10, 1.f) + 0.5f;
	
	if (aPos.y < 0.f) {
	//	Density = 2.f;
	}

	return Density;
}

static float GetDensity(XMVECTOR aPos) {
	XMFLOAT4 Pos;
	XMStoreFloat4(&Pos, aPos);

	return GetDensity(Pos);
}


