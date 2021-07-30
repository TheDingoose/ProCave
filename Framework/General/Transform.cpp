#include "pch.h"
#include "General/Transform.h"

void ApplyVelocity(flecs::entity e, cTransform& T, cVelocity& V) {
	XMStoreFloat4(&T.Translation, XMLoadFloat4(&T.Translation) + V.Speed);
	T.UpdateMatrix();
	V.Speed = XMVectorSet(0.f, 0.f, 0.f, 0.f);
}