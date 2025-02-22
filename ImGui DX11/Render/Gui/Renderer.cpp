﻿#include "Renderer.h"

bool show_menu = false;

void Renderer::Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd) {
    pDevice = device;
    pContext = context;
    window = hwnd;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(pDevice, pContext);

    ImGui::GetIO().Fonts->AddFontDefault();
    ImFontConfig font_cfg;
    font_cfg.GlyphExtraSpacing.x = 1.2;
    font::gameFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(font::TTSquaresCondensedBold, 14, 14, &font_cfg);
}

void Renderer::Render() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (show_menu) {
        DrawMenu();
    }

    ImGui::Render();
    Renderer::ToggleMenu();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::Hook(void** oPresent, void* HookPresent) {
    if (Backend::FindPresent()) {
        Backend::AttachPresent(oPresent, HookPresent);
    }
}

void Renderer::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (mainRenderTargetView) {
        mainRenderTargetView->Release();
        mainRenderTargetView = nullptr;
    }
}

void Renderer::ToggleMenu() {
    if (GetAsyncKeyState(VK_INSERT) & 1) {
        show_menu = !show_menu;
        ImGui::GetIO().MouseDrawCursor = show_menu;
    }
}