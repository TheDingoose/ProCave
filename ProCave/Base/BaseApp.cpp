#include "BaseApp.h"
#include "Graphics/Renderer.h"

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
	Cam->Position = XMVectorSetZ(Cam->Position, XMVectorGetZ(Cam->Position) - 0.01f);
	Cam->Position = XMVectorSetX(Cam->Position, XMVectorGetX(Cam->Position) - 0.01f);
	Cam->Position = XMVectorSetY(Cam->Position, XMVectorGetY(Cam->Position) + 0.01f);
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
