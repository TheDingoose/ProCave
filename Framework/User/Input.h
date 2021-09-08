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
	Key_MouseUnlock,
	Mouse_X,
	Mouse_Y
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

	float GetValue(Key key) {
		return Map->GetFloat(key);
	}

	float GetValueDelta(Key key) {
		return Map->GetFloatDelta(key);
	}

private:
	Input();
	gainput::InputManager GainputManager;
	gainput::DeviceId KeyboardId;
	gainput::DeviceId MouseId;
	gainput::InputMap* Map;
};

