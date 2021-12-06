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
	float LightStrength = 28.f;
	float PlayerLightStrength = 5.f;
	XMFLOAT4 SampleMod = XMFLOAT4(0.05f, 0.05f, 0.05f, 0.01f);
	XMFLOAT4 SampleOffset = XMFLOAT4(0.f, -10.f, 0.f, 0.f);
	float DensityLimit = 0.f;
	float Time = 0.f;
	float NormalSampleDistance = 1.0f;
	float TextureBlendOffset = 0.25f;
	float TextureBlendExponent = 2;
	float TextureBlendHeightStrength = 0.5f;
	float FogDistanceNear = 29.f;
	float FogDistance = 35.f;
	XMFLOAT4 FogColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.f);;
	XMFLOAT4 PlayerLightColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);
	XMFLOAT4 LightColor = XMFLOAT4(1.f, 1.f, 1.f, 1.f);;
	

private:
	MarchCubeSettings();

	unsigned int MaxGridSize = GridSize;
};

