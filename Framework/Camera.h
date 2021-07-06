#pragma once

#include <DirectXMath.h>


using namespace DirectX;

	class Camera
	{
	public:
		XMVECTOR Position = XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f);
		XMVECTOR Target = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);;
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		XMMATRIX VP;
		//XMMATRIX World;
		XMMATRIX View;
		XMMATRIX Projection;

		unsigned int* ScreenWidth;
		unsigned int* ScreenHeight;


		Camera(unsigned int* aWidth, unsigned int* aHeight) {
			ScreenWidth = aWidth;
			ScreenHeight = aHeight;

			View = XMMatrixLookAtLH(Position, Target, Up);
			Projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)*aWidth / *aHeight, 1.0f, 1000.0f);
			//World = XMMatrixIdentity();

			VP = View * Projection;

		}

		void Update() {
			View = XMMatrixLookAtLH(Position, Target, Up);
			Projection = XMMatrixPerspectiveFovLH(0.4f * 3.14f, (float)*ScreenWidth / *ScreenHeight, 1.0f, 1000.0f);
			//World = XMMatrixIdentity();

			VP = View * Projection;
		}
	};