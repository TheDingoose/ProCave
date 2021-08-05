#pragma once

#include <vector>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx11.h"
#include "imgui/backends/imgui_impl_win32.h"

#include "Rendering/DevUI.h"

#include <d3d11.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class DevUIDriver {
public:

	static DevUIDriver* get() {
		static DevUIDriver Instance;
		return &Instance;
	}

	void Update(float DeltaTime) {
		for (auto& i : Windows) {
			i->Update(DeltaTime);
		}
	};

	void Draw() {

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();

		if (ImGui::BeginMainMenuBar()) {
			for (auto& i : Windows) {
				if (i->Open) {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(.80f, 1.f, .34f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(.80f, 1.f, .42f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(.80f, 1.f, .5f));
					if (ImGui::Button(i->Name)) {
						i->Open = !i->Open;
					}
					ImGui::PopStyleColor(3);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.f, 0.f, .14f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(.80f, 1.f, .42f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(.80f, 1.f, .5f));
					if (ImGui::Button(i->Name)) {
						i->Open = !i->Open;
						i->Init();
					}
					ImGui::PopStyleColor(3);
				}
			}
			ImGui::EndMainMenuBar();
		}

		for (auto& i : Windows) {
			if (i->Open) {
				i->Draw();
			}
		}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	};

	std::vector<DevUI*> Windows;

	void SetupWindow(HWND hWnd, ID3D11Device* d3d11Device, ID3D11DeviceContext* d3d11DevCon) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		//Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(d3d11Device, d3d11DevCon);
	}

private:
	DevUIDriver() {

	};
};