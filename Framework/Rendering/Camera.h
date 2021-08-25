#pragma once
#include <DirectXMath.h>

#include "General/Transform.h"

using namespace DirectX;

	class Camera
	{
	public:

		cTransform Transform;
		XMVECTOR Target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX VP;
		//XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;

		unsigned int* ScreenWidth;
		unsigned int* ScreenHeight;


		Camera(unsigned int* aWidth, unsigned int* aHeight) {
			Transform.Translation.z = -0.1f;

			ScreenWidth = aWidth;
			ScreenHeight = aHeight;

			View = XMMatrixLookAtLH(XMLoadFloat4(&Transform.Translation), Target, Up);
			Projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)*aWidth / *aHeight, 1.0f, 1000.0f);
			//World = XMMatrixIdentity();

			VP = View * Projection;

		}

		void Update() {
			Transform.UpdateMatrix();
			Target = XMVector4Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), Transform.Transform);
			//XMStoreFloat4(&Step, XMVector4Transform(Change, Cam->Transform.Transform));

			//View = XMMatrixTranslation(Transform.Translation.x, Transform.Translation.y, Transform.Translation.z) * XMMatrixRotationRollPitchYaw(Transform.Rotation.x, Transform.Rotation.y, Transform.Rotation.z);
			//View *= ;

			//View = Transform.Transform;
			View = XMMatrixLookAtLH(XMLoadFloat4(&Transform.Translation), XMLoadFloat4(&Transform.Translation) + Target, Up);
			Projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)*ScreenWidth / *ScreenHeight, 1.0f, 1000.0f);
			//World = XMMatrixIdentity();

			VP = View * Projection;
		}

		~Camera() {
			ScreenHeight = nullptr;
			ScreenWidth = nullptr;
		}
	};