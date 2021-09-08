#include "MarchCubeSettings.h"

#include "Rendering/DevUIDriver.h"
#include "Graphics/Renderer.h"


MarchCubeSettings::MarchCubeSettings()
{
	Name = "MarchCubeSettings";
}

void MarchCubeSettings::Draw() {

	ImGui::Begin(Name);
	ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);
	
	if (ImGui::DragFloat("CubeSize", &CubeSize, 0.1f)) {
		Renderer::get()->cbPerObj.CubeSize = MarchCubeSettings::get()->CubeSize;
	};
	if (ImGui::DragInt("GridSize", (int*)&GridSize, 2.f, 0)) {

		if (GridSize > MaxGridSize) {
			Renderer::get()->CubePositions.resize(GridSize * GridSize * GridSize);

			D3D11_BUFFER_DESC pointBufferDesc;
			ZeroMemory(&pointBufferDesc, sizeof(pointBufferDesc));

			pointBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			pointBufferDesc.ByteWidth = sizeof(XMVECTOR) * Renderer::get()->CubePositions.size();
			pointBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			pointBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			pointBufferDesc.MiscFlags = 0;

			Renderer::get()->CubePosBuffer.resize(MarchCubeSettings::get()->GridSize);
			
			for (int i = 0; i < MaxGridSize; i++) {
				Renderer::get()->CubePosBuffer[i]->Release();
			}
			for (int i = 0; i < GridSize; i++) {
				Renderer::get()->d3d11Device->CreateBuffer(&pointBufferDesc, 0, &Renderer::get()->CubePosBuffer[i]);
			}
		}
	};
	if (ImGui::DragFloat("LightStrength", &LightStrength)) { //Move this later
		Renderer::get()->cbPerObj.LightStrength = MarchCubeSettings::get()->LightStrength;
	}; 
	if (ImGui::DragFloat4("SampleMod", &SampleMod.x, 0.005f)) {
		Renderer::get()->cbPerObj.SampleMod = MarchCubeSettings::get()->SampleMod;
	};
	if (ImGui::DragFloat4("SampleOffset", &SampleOffset.x, 0.5f)) {
		Renderer::get()->cbPerObj.SampleOffset = MarchCubeSettings::get()->SampleOffset;
	};
	if (ImGui::DragFloat("DensityLimit", &DensityLimit, 0.01f, -1.f, 1.f)) {
		Renderer::get()->cbPerObj.DensityLimit = MarchCubeSettings::get()->DensityLimit;
	};

	ImGui::End();
	return;
}