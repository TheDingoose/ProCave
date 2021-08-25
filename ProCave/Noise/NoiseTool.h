#pragma once

#include <d3d11.h>
//#include <dxgi.h>
//#include <DirectXMath.h>

#include "imgui/imgui.h"
#include "spdlog/spdlog.h"

#include "Rendering/DevUI.h"

constexpr int SampleSize = 64;

class NoiseTool : public DevUI {
public:
	NoiseTool();

	~NoiseTool() {
		Texture->Release();
		shaderResourceViewNoise->Release();
	}

	int Axis = 0;
	float SampleDistance = 1.f;
	float SourcePos[3] = { 0.f, 0.f, 0.f };
	
	ID3D11Texture2D* Texture = nullptr;
	ID3D11ShaderResourceView* shaderResourceViewNoise = nullptr;

	void Draw() override;

	float Buffer[SampleSize * SampleSize * 4];

	void SwapImage();


};