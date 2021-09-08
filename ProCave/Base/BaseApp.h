#pragma once
#include "Rendering/Camera.h"
#include "SH.h"
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
};

