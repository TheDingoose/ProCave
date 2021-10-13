#include "DebugSettings.h"

void DebugSettings::Draw()
{
	ImGui::Begin(Name);
	ImGui::SetWindowSize(ImVec2(400, 400), ImGuiCond_FirstUseEver);

	ImGui::Checkbox("Draw Physics Debug", &DrawPhysicsDebug);

	ImGui::End();
	return;
}
