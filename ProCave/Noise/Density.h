#pragma once
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

float GetDensity(XMFLOAT4 aPos) {
	float Density = 0.f;

	//Density -= aPos.y;
	

	Density += (fmod(((float)rand()) , 100.f) / 100.f) / aPos.y;

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


