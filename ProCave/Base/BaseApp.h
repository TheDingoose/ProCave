#pragma once
#include "Rendering/Camera.h"
class Renderer;

class BaseApp
{

public:

	BaseApp();

	void Init();

	void Load();

	void Tick();

	void UnLoad();

	void Exit();

	Camera* Cam;
	bool Shutdown = false;
};

