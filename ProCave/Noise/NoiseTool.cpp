#include "NoiseTool.h"

#include "Graphics/Renderer.h"
#include "Graphics/RenderHelper.h"
#include "Noise/Density.h"

using namespace DirectX;

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

	SwapImage();
}

void NoiseTool::Draw()
{
	ImGui::Begin(Name);
	ImGui::SetWindowSize(ImVec2(950, 600), ImGuiCond_FirstUseEver);
	if (ImGui::DragFloat3("SourcePos", SourcePos)) { SwapImage(); };
	if (ImGui::DragFloat("SampleDistance", &SampleDistance, 0.1f, 0.1f, 1000.f)) { SwapImage(); };

	if (ImGui::RadioButton("X", &Axis, 0)) { SwapImage(); }; ImGui::SameLine();
	if (ImGui::RadioButton("Y", &Axis, 1)) { SwapImage(); }; ImGui::SameLine();
	if (ImGui::RadioButton("Z", &Axis, 2)) { SwapImage(); };

	if (ImGui::Button("Refresh")) { SwapImage(); }

	ImGui::Image((void*)shaderResourceViewNoise, ImVec2(SampleSize, SampleSize));
	ImGui::Image((void*)shaderResourceViewNoise, ImVec2(128, 128));

	ImGui::End();
	return;
}

void NoiseTool::SwapImage()
{
	unsigned int WritePos = 0;

	for (int i = -(SampleSize / 2); i < SampleSize / 2; i++) {
		int k = -i;
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
}