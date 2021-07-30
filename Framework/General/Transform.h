#pragma once
#include <DirectXMath.h>

#include "flecs/flecs.h"

using namespace DirectX;

struct cTransform {
	XMFLOAT4 Translation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4 Size = XMFLOAT4(1.f, 1.f, 1.f, 0.f);
	XMFLOAT4 Rotation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	XMMATRIX Transform;

	void UpdateMatrix() {

		//s*r*t
		
		//XMMATRIX SizeMat(
		//	XMVectorGetX(Size), 0.f, 0.f, 0.f,
		//	0.f, XMVectorGetY(Size), 0.f, 0.f,
		//	0.f, 0.f, XMVectorGetZ(Size), 0.f,
		//	0.f, 0.f, 0.f, 0.f);
		//
		//XMMATRIX RotMat = XMMatrixRotationRollPitchYawFromVector(Rotation);
		
		Transform = 
			XMMatrixScalingFromVector(XMLoadFloat4(&Size)) * 
			XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&Rotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat4(&Translation));
	}
};

struct cVelocity {
	XMVECTOR Speed = XMVectorSet(0.f, 0.f, 0.f, 0.f);
};

void ApplyVelocity(flecs::entity e, cTransform& T, cVelocity& V);