#include "BaseApp.h"

#include "Graphics/Renderer.h"
#include "User/Input.h"
#include "Graphics/MarchCube.h"

#include "spdlog/spdlog.h"

#include "Noise/Ray.h"

BaseApp::BaseApp()
{
}

//Basic window start and setup will be done in this phase. but not here. 
// Otherwise Preloading
void BaseApp::Init()
{
	Renderer* aRenderer = Renderer::get();
	Cam = new Camera(&aRenderer->Width, &aRenderer->Height);
	aRenderer->SetVP(&Cam->VP, &Cam->Transform.Translation);
}

//Loading resources and assets, shaderloading goes here too
void BaseApp::Load()
{
	std::vector<Vertex> v =
	{
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
		Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
		Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
		Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
		Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	};

	std::vector<DWORD> indices = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	Model Coob(indices, v);

	Renderer::get()->InitializeModel(Coob);
	
	Renderer::get()->Models[0].Transform.Translation = XMFLOAT4(0.f, 0.f, 5.f, 0.f);


	
	//// THIS IS THE MARCHING CUBES
	//float sizee = 10.f;
	//sizee *= CubeSize;
	//XMVECTOR Worker = XMVectorZero();
	//int ModelNum = 0;
	//XMFLOAT4 Pos;
	//for (float x = -sizee; x < sizee; x += CubeSize) {
	//	Worker.m128_f32[0] = x;
	//	for (float y = -sizee; y < sizee; y += CubeSize) {
	//		Worker.m128_f32[1] = y;
	//		for (float z = -sizee; z < sizee; z += CubeSize) {
	//			Worker.m128_f32[2] = z;
	//
	//			MarchCube Dud(Worker);
	//
	//			if (Dud.Vertices.size() == 0) {
	//				continue;
	//			}
	//			//spdlog::info("------------------------------------");
	//			//spdlog::info("Case: {0:d} , {0:b}", Dud.Case);
	//			spdlog::info(ModelNum);
	//			//std::vector<DWORD> ind;
	//			int j = 0;
	//			//for (int i = 0; i < Dud.Vertices.size(); i += 3) {
	//			//	spdlog::info(j);
	//			//	ind.push_back(j++);
	//			//}
	//			//spdlog::info("------------------------------------");
	//			
	//			
	//
	//			Renderer::get()->InitializeModel(Model(Dud.Indices, Dud.Vertices));
	//			
	//			XMStoreFloat4(&Pos, Worker);
	//
	//			Renderer::get()->Models[ModelNum++].Transform.Translation = Pos;
	//		}
	//	}
	//}
	


	




}

XMVECTOR Velocity = XMVectorZero();

//Game update info goes here, more advanced functionality,
//such as messaging, happens in ProCave.cpp
void BaseApp::Tick(float Deltatime)
{
	//MarchCubeSettings::get()->Time += Deltatime;

	Cam->Transform.Translation.y -= 3.f;
	Cam->Update();

	XMVECTOR PlayerAcc = XMVectorZero();
	XMFLOAT4 Step;
	if (Input::get()->GetKey(Key_Forward)) {
		PlayerAcc.m128_f32[2] = 1.f;
	}
	if (Input::get()->GetKey(Key_Backward)) {
		PlayerAcc.m128_f32[2] = -1.f;
	}
	if (Input::get()->GetKey(Key_Right)) {
		PlayerAcc.m128_f32[0] = 1.f;
	}
	if (Input::get()->GetKey(Key_Left)) {
		PlayerAcc.m128_f32[0] = -1.f;
	}

	PlayerAcc = XMVector4Transform(PlayerAcc, Cam->Transform.Transform);
	
	

	PlayerAcc.m128_f32[1] = 0.f;
	XMVECTOR Offset = XMVector3Normalize(PlayerAcc);

	PlayerAcc = XMVector3Normalize(PlayerAcc) * 1.f;



	if (Input::get()->GetKey(Key_Up)) {
		PlayerAcc.m128_f32[1] = 2.f;
	}

	PlayerAcc.m128_f32[1] += -1.f;
	//Velocity = Velocity * Deltatime;

	Velocity += PlayerAcc;

	XMVECTOR Temp = XMLoadFloat4(&Cam->Transform.Translation);

	Ray::DensityCollisionVelocityTest(&Temp, &Velocity, Deltatime);
	XMStoreFloat4(&Cam->Transform.Translation, Temp);
	

	


	//if (Input::get()->GetKey(Key_Down)) {
	//	Cam->Transform.Translation.y -= 0.5f;
	//}
	


	

	//PlayerAcc *= 0.3f;


	if (!Input::get()->GetKey(Key_MouseUnlock) && GetForegroundWindow() == HandleWindow) {
		POINT cPos;
		GetCursorPos(&cPos);
		RECT rect = { NULL };
		GetWindowRect(HandleWindow, &rect);
		float Movex = -(rect.left + (rect.right - rect.left) / 2 - cPos.x) / 300.f;
		float Movey = -(rect.top + (rect.bottom - rect.top) / 2 - cPos.y) / 300.f;

		if (Cam->Transform.Rotation.x + Movey > -3.0f / 2.f && Cam->Transform.Rotation.x + Movey < 3.0f / 2.f) {
			Cam->Transform.Rotation.x += Movey;
		}
		Cam->Transform.Rotation.y += Movex;

		SetCursorPos(rect.left + (rect.right - rect.left) / 2, rect.top + (rect.bottom - rect.top) / 2);
	}

	Cam->Transform.Translation.y += 3.f;
	Cam->Update();
	XMStoreFloat4(&Renderer::get()->Models[0].Transform.Translation, Ray::Test(XMLoadFloat4(&Cam->Transform.Translation), Cam->Target));
	

	float Rotat = 0.01f;

	Renderer::get()->Models[0].Transform.Rotation.x += Rotat;
	Renderer::get()->Models[0].Transform.Rotation.y += Rotat;
	Renderer::get()->Models[0].Transform.Rotation.z += Rotat;
}

//Free all currently claimed resources
//Unload any loaded assets
void BaseApp::UnLoad()
{
}

//Kill screen, And stop final processes
void BaseApp::Exit()
{
}
