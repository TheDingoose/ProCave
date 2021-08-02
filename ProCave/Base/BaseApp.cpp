#include "BaseApp.h"
#include "Graphics/Renderer.h"
#include "User/Input.h"

BaseApp::BaseApp()
{
}

//Basic window start and setup will be done in this phase. but not here. 
// Otherwise Preloading
void BaseApp::Init()
{
	Renderer* aRenderer = Renderer::get();
	Cam = new Camera(&aRenderer->Width, &aRenderer->Height);
	aRenderer->SetVP(&Cam->VP);
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

	std::vector<Vertex> sv =
	{
		Vertex(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f),
		Vertex(-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
		Vertex(0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f),
		Vertex(0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f),
	};

	std::vector<DWORD> sindices = {
	0, 1, 2,
	0, 2, 3,
	};

	Model Square(sindices, sv);

	Renderer::get()->InitializeModel(Square);
	Renderer::get()->InitializeModel(Coob);

	Renderer::get()->Models[0].Transform.Translation = XMFLOAT4(0.f, 0.f, 5.f, 0.f);
	Renderer::get()->Models[1].Transform.Translation = XMFLOAT4(4.f, 0.f, 5.f, 0.f);
}

//Game update info goes here, more advanced functionality,
//such as messaging, happens in ProCave.cpp
void BaseApp::Tick()
{
	if (Input::get()->GetKey(Forward)) {
		Cam->Transform.Translation.z -= 0.05f;
	}
	if (Input::get()->GetKey(Backward)) {
		Cam->Transform.Translation.z += 0.05f;
	}
	if (Input::get()->GetKey(Right)) {
		Cam->Transform.Translation.x -= 0.05f;
	}
	if (Input::get()->GetKey(Left)) {
		Cam->Transform.Translation.x += 0.05f;
	}
	if (Input::get()->GetKey(Up)) {
		Cam->Transform.Translation.y -= 0.05f;
	}
	if (Input::get()->GetKey(Down)) {
		Cam->Transform.Translation.y += 0.05f;
	}
	if (Input::get()->GetKey(CamLeft)) {
		Cam->Transform.Rotation.y += 0.05f;
	}
	if (Input::get()->GetKey(CamRight)) {
		Cam->Transform.Rotation.y -= 0.05f;
	}
	if (Input::get()->GetKey(CamUp)) {
		Cam->Transform.Rotation.x += 0.05f;
	}
	if (Input::get()->GetKey(CamDown)) {
		Cam->Transform.Rotation.x -= 0.05f;
	}

	Cam->Update();

	float Rotat = 0.01f;

	Renderer::get()->Models[0].Transform.Rotation.x += Rotat;
	Renderer::get()->Models[0].Transform.Rotation.y += Rotat;
	Renderer::get()->Models[0].Transform.Rotation.z += Rotat;

	Renderer::get()->Models[1].Transform.Rotation.x += Rotat;
	Renderer::get()->Models[1].Transform.Rotation.y += Rotat;
	Renderer::get()->Models[1].Transform.Rotation.z += Rotat;
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
