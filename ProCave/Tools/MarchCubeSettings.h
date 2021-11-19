#pragma once

#include <DirectXMath.h>

#include "Rendering/DevUI.h"

using namespace DirectX;

class MarchCubeSettings : public DevUI
{
public:

	static MarchCubeSettings* get() {
		static MarchCubeSettings Instance;
		return &Instance;
	}

	

	void Draw() override;




	float CubeSize = 1.f;
	unsigned int GridSize = 35;
	float LightStrength = 30.f;
	XMFLOAT4 SampleMod = XMFLOAT4(0.05f, 0.05f, 0.05f, 0.01f);
	XMFLOAT4 SampleOffset = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
	float DensityLimit = 0.f;
	float Time = 0.f;
	float NormalSampleDistance = 1.0f;

private:
	MarchCubeSettings();

	unsigned int MaxGridSize = GridSize;
};

