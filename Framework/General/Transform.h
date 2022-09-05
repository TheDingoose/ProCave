#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct cTransform {
	XMFLOAT4 Translation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	XMFLOAT4 Scale = XMFLOAT4(1.f, 1.f, 1.f, 0.f);
	XMFLOAT4 Rotation = XMFLOAT4(0.f, 0.f, 0.f, 0.f);

	XMMATRIX Transform;

	void UpdateMatrix() {

		//s*r*t
		
		//XMMATRIX SizeMat(
		//	XMVectorGetX(Scale), 0.f, 0.f, 0.f,
		//	0.f, XMVectorGetY(Scale), 0.f, 0.f,
		//	0.f, 0.f, XMVectorGetZ(Scale), 0.f,
		//	0.f, 0.f, 0.f, 0.f);
		//
		//XMMATRIX RotMat = XMMatrixRotationRollPitchYawFromVector(Rotation);
		
		Transform = 
			XMMatrixScalingFromVector(XMLoadFloat4(&Scale)) * 
			XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&Rotation)) *
			XMMatrixTranslationFromVector(XMLoadFloat4(&Translation));

		//Transform =
		//	XMMatrixTranslationFromVector(XMLoadFloat4(&Translation)) *
		//	XMMatrixRotationRollPitchYawFromVector(XMLoadFloat4(&Rotation)) *
		//	XMMatrixScalingFromVector(XMLoadFloat4(&Scale));
	}
};

struct cVelocity {
	XMVECTOR Speed = XMVectorSet(0.f, 0.f, 0.f, 0.f);
};

//void ApplyVelocity(flecs::entity e, cTransform& T, cVelocity& V);