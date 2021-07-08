#pragma once
#include <DirectXMath.h>

#include "flecs/flecs.h"

using namespace DirectX;

struct cTransform {
	XMVECTOR Translation = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	XMVECTOR Size = XMVectorSet(1.f, 1.f, 1.f, 0.f);
	XMVECTOR Rotation = XMVectorSet(0.f, 0.f, 0.f, 0.f);

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
			XMMatrixScalingFromVector(Size) * 
			XMMatrixRotationRollPitchYawFromVector(Rotation) * 
			XMMatrixTranslationFromVector(Translation);
	}
};

struct cVelocity {
	XMVECTOR Speed = XMVectorSet(0.f, 0.f, 0.f, 0.f);
};

void ApplyVelocity(flecs::entity e, cTransform& T, cVelocity& V) {
	T.Translation += V.Speed;
	T.UpdateMatrix();
	V.Speed = XMVectorSet(0.f, 0.f, 0.f, 0.f);
}