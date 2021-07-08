#include "pch.h"
#include "Input.h"



Input::Input()
{
	KeyboardId = GainputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	Map = new gainput::InputMap(GainputManager);


	//Mapping Keys
	Map->MapBool(Key::Confirm, KeyboardId, gainput::KeyReturn);
	Map->MapBool(Key::Forward, KeyboardId, gainput::KeyW);
	Map->MapBool(Key::Backward, KeyboardId, gainput::KeyS);
	Map->MapBool(Key::Left, KeyboardId, gainput::KeyA);
	Map->MapBool(Key::Right, KeyboardId, gainput::KeyD);
	Map->MapBool(Key::Up, KeyboardId, gainput::KeySpace);
	Map->MapBool(Key::Down, KeyboardId, gainput::KeyShiftL);

}
