#pragma once
#include "Rendering/Camera.h"
#include "SH.h"

#include "reactphysics3d/reactphysics3d.h"
using namespace reactphysics3d;

class Renderer;

class BaseApp
{

public:

	BaseApp();

	void Init();

	void Load();

	void Tick(float Deltatime);

	void UnLoad();

	void Exit();

	HWND HandleWindow;
	Camera* Cam;
	bool Shutdown = false;

	PhysicsCommon physicsCommon;
	PhysicsWorld* world ;
};