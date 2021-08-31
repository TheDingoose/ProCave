#include "NoiseTool.h"

#include <d3dcompiler.h>

#include "Graphics/Renderer.h"
#include "Graphics/RenderHelper.h"
#include "Noise/Density.h"
#include "Tools/MarchCubeSettings.h"

using namespace DirectX;

struct GPUConstants {
	int SampleSize = 64;
	int Axis = 0;
	float SampleDistance = 1.f;
	int pad2;
	float SourcePos[3] = { 0.f, 0.f, 0.f };
	int pad1;
	XMFLOAT4 Offset;
	XMFLOAT4 Mod;
};

GPUConstants GPUBuff;


NoiseTool::NoiseTool()
{
	Name = "NoiseTool";

	float Buffer[SampleSize * SampleSize * 4];

	for (int i = 0; i < SampleSize * SampleSize * 4; i++) {
		Buffer[i] = 0.5f;
	}

	D3D11_SUBRESOURCE_DATA BufferData;
	BufferData.pSysMem = Buffer;
	BufferData.SysMemPitch = SampleSize * 4 * sizeof(float);
	BufferData.SysMemSlicePitch = 0;

	//make an image
	HRESULT hr;

	DXGI_SAMPLE_DESC MultiSample;
	MultiSample.Count = 1;
	MultiSample.Quality = 0;

	D3D11_TEXTURE2D_DESC ImageDesc;
	ImageDesc.Width = SampleSize;
	ImageDesc.Height = SampleSize;
	ImageDesc.MipLevels = 1;
	ImageDesc.ArraySize = 1; //Do we want to make this size as well?
	ImageDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ImageDesc.SampleDesc = MultiSample;
	ImageDesc.Usage = D3D11_USAGE_DYNAMIC;
	ImageDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;//| D3D11_BIND_UNORDERED_ACCESS;
	ImageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;// | D3D11_CPU_ACCESS_READ;
	ImageDesc.MiscFlags = 0;

	hr = Renderer::get()->d3d11Device->CreateTexture2D(&ImageDesc, &BufferData, &Texture);
	CheckError(hr, "Problem Creating Noise Texture");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = ImageDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Renderer::get()->d3d11Device->CreateShaderResourceView(Texture, &srvDesc, &shaderResourceViewNoise);

	//============================================GPU============================================
	ID3DBlob* compilationMsgs = nullptr;
	hr = D3DCompileFromFile(L"MarchCubeToTexture.hlsl", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", 0, 0, &GPUCS_Buffer, &compilationMsgs);
	CheckError(hr, compilationMsgs);

	hr = Renderer::get()->d3d11Device->CreateComputeShader(GPUCS_Buffer->GetBufferPointer(), GPUCS_Buffer->GetBufferSize(), NULL, &GPUShader);
	CheckError(hr, "Error While compiling Cube Vertex Shader!");

	//Create the Constant Buffer
	D3D11_BUFFER_DESC cbbd;
	ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

	cbbd.Usage = D3D11_USAGE_DEFAULT;
	cbbd.ByteWidth = sizeof(GPUConstants);
	cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbbd.CPUAccessFlags = 0;
	cbbd.MiscFlags = 0;

	hr = Renderer::get()->d3d11Device->CreateBuffer(&cbbd, NULL, &GPUConstBuffer);

	ImageDesc.Width = SampleSize;
	ImageDesc.Height = SampleSize;
	ImageDesc.MipLevels = 1;
	ImageDesc.ArraySize = 1; //Do we want to make this size as well?
	ImageDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	ImageDesc.SampleDesc = MultiSample;
	ImageDesc.Usage = D3D11_USAGE_DEFAULT;
	ImageDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;//| D3D11_BIND_UNORDERED_ACCESS;
	ImageDesc.CPUAccessFlags = 0;// | D3D11_CPU_ACCESS_READ;
	ImageDesc.MiscFlags = 0;

	hr = Renderer::get()->d3d11Device->CreateTexture2D(&ImageDesc, &BufferData, &GPUTexture);
	CheckError(hr, "Problem Creating GPU Noise Texture");

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;

	UAVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;

	Renderer::get()->d3d11DevCon->CSSetShader(GPUShader, 0, 0);
	Renderer::get()->d3d11Device->CreateUnorderedAccessView(GPUTexture, &UAVDesc, &GPUshaderResourceViewNoise);
	
	Renderer::get()->d3d11Device->CreateShaderResourceView(GPUTexture, &srvDesc, &GPUNoiseViewer);
	//Renderer::get()->d3d11DevCon->CSSetShaderResources(0, 1, &GPUshaderResourceViewNoise);
	

	SwapImage();
}

void NoiseTool::Draw()
{
	ImGui::Begin(Name);
	ImGui::SetWindowSize(ImVec2(950, 600), ImGuiCond_FirstUseEver);
	if (ImGui::DragFloat3("SourcePos", SourcePos)) { GPUBuff.SourcePos[0] = SourcePos[0]; GPUBuff.SourcePos[1] = SourcePos[1]; GPUBuff.SourcePos[2] = SourcePos[2]; SwapImage(); };
	if (ImGui::DragFloat("SampleDistance", &SampleDistance, 0.1f, 0.1f, 1000.f)) { GPUBuff.SampleDistance = SampleDistance; SwapImage(); };

	if (ImGui::RadioButton("X", &Axis, 0)) { GPUBuff.Axis = 0; SwapImage(); }; ImGui::SameLine();
	if (ImGui::RadioButton("Y", &Axis, 1)) { GPUBuff.Axis = 1; SwapImage(); }; ImGui::SameLine();
	if (ImGui::RadioButton("Z", &Axis, 2)) { GPUBuff.Axis = 2; SwapImage(); };

	if (ImGui::Button("Refresh")) { SwapImage(); }

	ImGui::Text("Native Size CPU");
	ImGui::Image((void*)shaderResourceViewNoise, ImVec2(SampleSize, SampleSize));
	ImGui::Text("CPU Noise Function - GPU Noise Function");
	ImGui::Image((void*)shaderResourceViewNoise, ImVec2(128, 128)); ImGui::SameLine();
	ImGui::Image(GPUNoiseViewer, ImVec2(128, 128));

	ImGui::End();
	return;
}

void NoiseTool::SwapImage()
{
	//============================================CPU============================================

	unsigned int WritePos = 0;

	for (int i = -(SampleSize / 2); i < SampleSize / 2; i++) {
		for (int j = -(SampleSize / 2); j < SampleSize / 2; j++) {

			XMFLOAT4 SamplePos(SourcePos[0], SourcePos[1], SourcePos[2], 0.f);

			if (Axis == 0) {		//along X
				SamplePos.y -= (float)i * SampleDistance;
				SamplePos.z -= (float)j * SampleDistance;
			}
			else if (Axis == 1) {	//along Y
				SamplePos.x += (float)i * SampleDistance;
				SamplePos.z -= (float)j * SampleDistance;
			}
			else if (Axis == 2) {	//along Z
				SamplePos.x += (float)j * SampleDistance;
				SamplePos.y -= (float)i * SampleDistance;
			}

			Buffer[WritePos++] = GetDensity(SamplePos);
			Buffer[WritePos++] = GetDensity(SamplePos);
			Buffer[WritePos++] = GetDensity(SamplePos);
			Buffer[WritePos++] = 1.f;
		}
	}

	//Set texture as
	D3D11_MAPPED_SUBRESOURCE BufferData;
	ZeroMemory(&BufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
	D3D11CalcSubresource(0, 0, 1);
	HRESULT hr = Renderer::get()->d3d11DevCon->Map(Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &BufferData);
	CheckError(hr, "Swap texture on noise regenerate");

	memcpy(BufferData.pData, Buffer, SampleSize * SampleSize * 4 * sizeof(float));

	Renderer::get()->d3d11DevCon->Unmap(Texture, 0);

	//============================================GPU============================================
	Renderer::get()->d3d11DevCon->CSSetShader(GPUShader, 0, 0);
	Renderer::get()->d3d11DevCon->CSSetUnorderedAccessViews(0, 1, &GPUshaderResourceViewNoise, 0);
	GPUBuff.Offset = MarchCubeSettings::get()->SampleOffset;
	GPUBuff.Mod = MarchCubeSettings::get()->SampleMod;


	Renderer::get()->d3d11DevCon->UpdateSubresource(GPUConstBuffer, 0, NULL, &GPUBuff, 0, 0);
	Renderer::get()->d3d11DevCon->CSSetConstantBuffers(0, 1, &GPUConstBuffer);
	Renderer::get()->d3d11DevCon->Dispatch(1, 1, 1);
	ID3D11UnorderedAccessView* s = nullptr;
	Renderer::get()->d3d11DevCon->CSSetUnorderedAccessViews(0, 1, &s, 0);
}
	
	
