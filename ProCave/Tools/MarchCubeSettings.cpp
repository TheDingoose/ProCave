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
	if (ImGui::DragFloat4("SampleMod", &SampleMod.x, 0.005f)) {
		Renderer::get()->cbPerObj.SampleMod = SampleMod;
	};
	if (ImGui::DragFloat4("SampleOffset", &SampleOffset.x, 0.5f)) {
		Renderer::get()->cbPerObj.SampleOffset = SampleOffset;
	};
	if (ImGui::DragFloat("DensityLimit", &DensityLimit, 0.01f, -1.f, 1.f)) {
		Renderer::get()->cbPerObj.DensityLimit = DensityLimit;
	};
	if (ImGui::DragFloat("NormalSampleDistance", &NormalSampleDistance, 0.001f, 0.f, 100.f)) {
		Renderer::get()->cbPerObj.NormalSampleDistance = NormalSampleDistance;
	};

	if (ImGui::DragFloat("LightStrength", &LightStrength)) { //Move this later
		Renderer::get()->cbPerObj.LightStrength = LightStrength;
	};
	if (ImGui::DragFloat("PlayerLightStrength", &PlayerLightStrength)) { //Move this later
		Renderer::get()->cbPerObj.PlayerLightStrength = PlayerLightStrength;
	
	};
	if (ImGui::ColorEdit4("PlayerLightColor", &PlayerLightColor.x)) {
		Renderer::get()->cbPerObj.PlayerLightColor = PlayerLightColor;
	};
	if (ImGui::ColorEdit4("LightColor", &LightColor.x)) {
		Renderer::get()->cbPerObj.LightColor = LightColor;
	};

	if (ImGui::DragFloat("FogDistanceNear", &FogDistanceNear)) {
		Renderer::get()->cbPerObj.FogDistanceNear = FogDistanceNear;
	};
	if (ImGui::DragFloat("FogDistanceFar", &FogDistance)) {
		Renderer::get()->cbPerObj.FogDistance = FogDistance;
	};
	if (ImGui::ColorEdit4("FogColor", &FogColor.x)) {
		Renderer::get()->cbPerObj.FogColor = FogColor;
	};

	if (ImGui::DragFloat("BlendOffset", &TextureBlendOffset, 0.01f, 0.f, 0.5f)) {
		Renderer::get()->cbPerObj.TextureBlendOffset = TextureBlendOffset;
	};
	if (ImGui::DragFloat("BlendExponent", &TextureBlendExponent, 0.5f, 1.f, 8.f)) {
		Renderer::get()->cbPerObj.TextureBlendExponent = TextureBlendExponent;
	};
	if (ImGui::DragFloat("BlendHeightStrength", &TextureBlendHeightStrength, 0.01f, 0.f, 0.99f)) {
		Renderer::get()->cbPerObj.TextureBlendHeightStrength = TextureBlendHeightStrength;
	};

	ImGui::End();
	return;
}

XMVECTOR GridRound(XMVECTOR Pos) {
	Pos.m128_f32[0] = round(Pos.m128_f32[0] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	Pos.m128_f32[1] = round(Pos.m128_f32[1] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	Pos.m128_f32[2] = round(Pos.m128_f32[2] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	return Pos;
}

XMVECTOR GridFloor(XMVECTOR Pos) {
	Pos.m128_f32[0] = floor(Pos.m128_f32[0] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	Pos.m128_f32[1] = floor(Pos.m128_f32[1] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	Pos.m128_f32[2] = floor(Pos.m128_f32[2] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	return Pos;
}

XMVECTOR GridCeil(XMVECTOR Pos) {
	Pos.m128_f32[0] = ceil(Pos.m128_f32[0] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	Pos.m128_f32[1] = ceil(Pos.m128_f32[1] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	Pos.m128_f32[2] = ceil(Pos.m128_f32[2] / MarchCubeSettings::get()->CubeSize) * MarchCubeSettings::get()->CubeSize;
	return Pos;
}