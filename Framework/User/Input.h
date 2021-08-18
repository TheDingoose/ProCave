#pragma once
#include "gainput/gainput.h"

enum Key {
	Key_Confirm,
	Key_Cancel,
	Key_Forward,
	Key_Backward,
	Key_Left,
	Key_Right,
	Key_Up,
	Key_Down,
	Key_CamLeft,
	Key_CamRight,
	Key_CamUp,
	Key_CamDown,
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

