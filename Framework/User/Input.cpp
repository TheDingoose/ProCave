#include "pch.h"
#include "Input.h"



Input::Input()
{
	KeyboardId = GainputManager.CreateDevice<gainput::InputDeviceKeyboard>();
	Map = new gainput::InputMap(GainputManager);


	//Mapping Keys
	Map->MapBool(Key::Key_Confirm, KeyboardId, gainput::KeyReturn);
	Map->MapBool(Key::Key_Forward, KeyboardId, gainput::KeyW);
	Map->MapBool(Key::Key_Backward, KeyboardId, gainput::KeyS);
	Map->MapBool(Key::Key_Left, KeyboardId, gainput::KeyA);
	Map->MapBool(Key::Key_Right, KeyboardId, gainput::KeyD);
	Map->MapBool(Key::Key_Up, KeyboardId, gainput::KeySpace);
	Map->MapBool(Key::Key_Down, KeyboardId, gainput::KeyShiftL);
	Map->MapBool(Key::Key_CamLeft, KeyboardId, gainput::KeyLeft);
	Map->MapBool(Key::Key_CamRight, KeyboardId, gainput::KeyRight);
	Map->MapBool(Key::Key_CamUp, KeyboardId, gainput::KeyUp);
	Map->MapBool(Key::Key_CamDown, KeyboardId, gainput::KeyDown);


}
