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
}

//Game update info goes here, more advanced functionality,
//such as messaging, happens in ProCave.cpp
void BaseApp::Tick()
{
	XMVECTOR Manouvre = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);;

	if (Input::get()->GetKeyDown(Forward)) {
		Manouvre = XMVectorSetZ(Manouvre, XMVectorGetZ(Manouvre) + 0.01f);
	}
	if (Input::get()->GetKeyDown(Backward)) {
		Manouvre = XMVectorSetZ(Manouvre, XMVectorGetZ(Manouvre) - 0.01f);
	}
	if (Input::get()->GetKeyDown(Right)) {
		Manouvre = XMVectorSetX(Manouvre, XMVectorGetX(Manouvre) + 0.01f);
	}
	if (Input::get()->GetKeyDown(Left)) {
		Manouvre = XMVectorSetX(Manouvre, XMVectorGetX(Manouvre) - 0.01f);
	}
	if (Input::get()->GetKeyDown(Up)) {
		Manouvre = XMVectorSetY(Manouvre, XMVectorGetY(Manouvre) + 0.01f);
	}
	if (Input::get()->GetKeyDown(Down)) {
		Manouvre = XMVectorSetY(Manouvre, XMVectorGetY(Manouvre) - 0.01f);
	}



	Cam->Position = Cam->Position + Manouvre;
	Cam->Update();
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
