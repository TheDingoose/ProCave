#pragma once
#include <vector>
#include <DirectXMath.h>
#include "perlin/PerlinNoise.hpp"
using namespace DirectX;
siv::PerlinNoise perlin;

float GetDensity(XMFLOAT4 aPos) {
	float Density = 0.f;

	//Density -= aPos.y;
	

	//Density += (fmod(((float)rand()) , 100.f) / 100.f) / aPos.y;
	Density += std::fmin(perlin.noise3D(aPos.x, aPos.y, aPos.z), 1.f);
	
	if (aPos.y < 0) {
		Density = 1.f;
	}

	return Density;
}

float GetDensity(XMVECTOR aPos) {
	XMFLOAT4 Pos;
	XMStoreFloat4(&Pos, aPos);

	return GetDensity(Pos);
}


