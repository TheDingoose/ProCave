#pragma once
#include "Rendering/Camera.h"
#include "SH.h"

#include "reactphysics3d/reactphysics3d.h"
using namespace reactphysics3d;

#include "Player.h"
#include <vector>
#include "Gadgets/ThrowLight.h"

class Renderer;
class EnvironmentCollisionHandler;
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
	bool MouseLock = true;

	EnvironmentCollisionHandler* EnvCollision;
	
	Player* ThePlayer;
	std::vector<ThrowLight*> Lights;

	PhysicsCommon physicsCommon;
	PhysicsWorld* world ;
};