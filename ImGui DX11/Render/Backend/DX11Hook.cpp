#include "DX11Hook.h"

typedef HRESULT(__stdcall* IDXGISwapChainPresent)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
static IDXGISwapChainPresent fnIDXGISwapChainPresent;

Present oPresent = NULL;
HWND window = NULL;
WNDPROC oWndProc = NULL;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
        return true;
    }
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


bool init = false;
HRESULT __stdcall Backend::HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    if (!init) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice))) {
            pDevice->GetImmediateContext(&pContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
            pBackBuffer->Release();
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);

            Renderer::Init(pDevice, pContext, window);
            init = true;
        }
        else {
            return oPresent(pSwapChain, SyncInterval, Flags);
        }
    }

    Renderer::Render();

    return oPresent(pSwapChain, SyncInterval, Flags);
}


bool Backend::FindPresent() {
    WNDCLASSEX wc;
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProc;
    wc.lpszClassName = "DX11";

    RegisterClassEx(&wc);

    HWND wnd = CreateWindow(wc.lpszClassName, 0, WS_DISABLED, 0, 0, 0, 0, NULL, NULL, NULL, nullptr);

    IDXGISwapChain* pSwapChain;

    D3D_FEATURE_LEVEL featureLevel;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = wnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ID3D11DeviceContext* pContext;
    ID3D11Device* pDevice;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0, nullptr, 1, D3D11_SDK_VERSION,
        &swapChainDesc, &pSwapChain, &pDevice, &featureLevel, &pContext)) &&
        FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
            &swapChainDesc, &pSwapChain, &pDevice, &featureLevel, &pContext)))
    {
        DestroyWindow(swapChainDesc.OutputWindow);
        UnregisterClass(wc.lpszClassName, GetModuleHandle(nullptr));
        return false;
    }

    const DWORD_PTR* pSwapChainVtable = reinterpret_cast<DWORD_PTR*>(pSwapChain);
    pSwapChainVtable = reinterpret_cast<DWORD_PTR*>(pSwapChainVtable[0]);
    fnIDXGISwapChainPresent = reinterpret_cast<IDXGISwapChainPresent>(pSwapChainVtable[8]);

    pSwapChain->Release();
    pDevice->Release();
    pContext->Release();

    DestroyWindow(swapChainDesc.OutputWindow);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return true;
}


bool Backend::AttachPresent(void** original, void* function) {
    if (!fnIDXGISwapChainPresent) return false;

    if (MH_CreateHook(reinterpret_cast<void*>(fnIDXGISwapChainPresent), function, original) != MH_OK ||
        MH_EnableHook(reinterpret_cast<void*>(fnIDXGISwapChainPresent)) != MH_OK) {
        return false;
    }

    return true;
}


bool Backend::DetachPresent() {
    if (MH_DisableHook(reinterpret_cast<void*>(fnIDXGISwapChainPresent)) != MH_OK ||
        MH_Uninitialize() != MH_OK) {
        return false;
    }

    return true;
}