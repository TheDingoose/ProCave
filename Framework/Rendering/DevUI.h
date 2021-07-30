#pragma once

#include "imgui/imgui.h"

class DevUI {
	public:

	DevUI(const char* aName) : Name(aName) {};
	DevUI() {};
	virtual void Init() {};
	virtual void Update(float DeltaTime) {};
	virtual void Draw() = 0;
	
	const char* Name = "Undefined";

	bool Open = false;
};
