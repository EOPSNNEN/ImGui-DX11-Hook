#include "base/includes.h"

bool InitHook() {
    createConsole();

    if (MH_Initialize() != MH_OK) {
        return false;
    }
    return true;
}

void Run() {
    if (InitHook()) { LOG("Init success"); }
    Renderer::Hook((void**)&oPresent, Backend::HookPresent);
}


BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    DisableThreadLibraryCalls(hMod);

    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Run, hMod, 0, nullptr);
            break;
        case DLL_PROCESS_DETACH:
            Renderer::Shutdown();
            break;
        }
    return TRUE;
}
