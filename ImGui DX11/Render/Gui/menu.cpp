#include "menu.h"

void DrawMenu() {
    ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 200), ImVec2(800, 600));

    if (ImGui::Begin("ImGui DX11 Window"), ImGuiWindowFlags_NoCollapse) {
        ImGui::End();
    }
}