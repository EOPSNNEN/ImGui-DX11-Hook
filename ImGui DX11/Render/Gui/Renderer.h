#pragma once
#include "menu.h"
#include "../Backend/DX11Hook.h"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_impl_dx11.h"

class Renderer {
public:
    static void Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd);
    static void Render();
    static void Hook(void** oPresent, void* HookPresent);
    static void Shutdown();
    static void ToggleMenu();
};
