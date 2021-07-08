#pragma once
#include "gainput/gainput.h"

enum Key {
	Confirm,
	Cancel,
	Forward,
	Backward,
	Left,
	Right,
	Up,
	Down
};

class Input
{
public:
	static Input* get() {
		static Input Instance;

		return &Instance;
	}

	gainput::InputManager* GetManager() {
		return &GainputManager;
	}

	bool GetKey(Key key) {
		return Map->GetBool(key);
	}

private:
	Input();
	gainput::InputManager GainputManager;
	gainput::DeviceId KeyboardId;
	gainput::InputMap* Map;
};

