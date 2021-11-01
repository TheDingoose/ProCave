#pragma once

#include "Rendering/DevUI.h"


class DebugSettings : public DevUI
{
public:

	static DebugSettings* get() {
		static DebugSettings Instance;
		return &Instance;
	}

	void Draw() override;

	bool DrawPhysicsDebug = false;

private:
	DebugSettings() { Name = "DebugSettings"; };
};

