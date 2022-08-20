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

	bool DrawPhysicsDebug = true;

private:
	DebugSettings() { Name = "DebugSettings"; };
};

