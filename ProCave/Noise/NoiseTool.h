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
	ID3D11Texture2D* GPUTexture = nullptr;
	ID3D11UnorderedAccessView* GPUshaderResourceViewNoise = nullptr;
	ID3D11ShaderResourceView* GPUNoiseViewer = nullptr;
	ID3D11Buffer* GPUConstBuffer;
	ID3DBlob* GPUCS_Buffer;
	ID3D11ComputeShader* GPUShader;

	void Draw() override;

	float Buffer[SampleSize * SampleSize * 4];

	void SwapImage();


};