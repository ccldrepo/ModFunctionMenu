#include "Renderer.h"

#include <dxgi.h>

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "../Menu.h"

namespace ImGui
{
    struct WndProcHook
    {
        static LRESULT thunk(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            if (uMsg == WM_KILLFOCUS) {
                auto& io = ImGui::GetIO();
                io.ClearInputKeys();
            }
            return func(hWnd, uMsg, wParam, lParam);
        }

        static inline WNDPROC func;
    };

    struct D3DInitHook
    {
        static void thunk()
        {
            func();

            const auto renderer = RE::BSGraphics::Renderer::GetSingleton();
            if (!renderer) {
                SKSE::log::error("couldn't find renderer");
                return;
            }

            const auto swapChain = (IDXGISwapChain*)renderer->data.renderWindows[0].swapChain;
            if (!swapChain) {
                SKSE::log::error("couldn't find swapChain");
                return;
            }

            DXGI_SWAP_CHAIN_DESC desc{};
            if (FAILED(swapChain->GetDesc(std::addressof(desc)))) {
                SKSE::log::error("IDXGISwapChain::GetDesc failed.");
                return;
            }

            const auto device = (ID3D11Device*)renderer->data.forwarder;
            const auto context = (ID3D11DeviceContext*)renderer->data.context;

            SKSE::log::info("Initializing ImGui...");

            ImGui::CreateContext();

            auto& io = ImGui::GetIO();
            io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
            io.IniFilename = nullptr;

            if (!ImGui_ImplWin32_Init(desc.OutputWindow)) {
                SKSE::log::error("ImGui initialization failed (Win32)");
                return;
            }

            if (!ImGui_ImplDX11_Init(device, context)) {
                SKSE::log::error("ImGui initialization failed (DX11)");
                return;
            }

            initialized.store(true);

            SKSE::log::info("ImGui initialized.");

            WndProcHook::func = reinterpret_cast<WNDPROC>(
                SetWindowLongPtrA(desc.OutputWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook::thunk)));
            if (!WndProcHook::func) {
                SKSE::log::error("SetWindowLongPtrA failed!");
            }
        }

        static inline REL::Relocation<decltype(thunk)> func;
    };

    struct DXGIPresentHook
    {
        static void thunk(std::uint32_t a_timer)
        {
            func(a_timer);

            if (!initialized.load()) {
                return;
            }

            if (!shouldRender.load()) {
                return;
            }

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            {
                // trick imgui into rendering at game's real resolution (ie. if upscaled with Display Tweaks)
                static const auto screenSize = RE::BSGraphics::Renderer::GetScreenSize();

                auto& io = ImGui::GetIO();
                io.DisplaySize.x = static_cast<float>(screenSize.width);
                io.DisplaySize.y = static_cast<float>(screenSize.height);
            }
            ImGui::NewFrame();
            {
                Menu::GetSingleton()->Draw();
            }
            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }

        static inline REL::Relocation<decltype(thunk)> func;
    };

    void Install()
    {
        REL::Relocation<std::uintptr_t> target{ REL::RelocationID(75595, 77226, 0xDC5530),
            REL::VariantOffset(0x9, 0x275, 0x9) };
        SKSE::stl::write_thunk_call<D3DInitHook>(target);
        SKSE::log::info("Hooked 0x{:016X} call.", target.address());

        REL::Relocation<std::uintptr_t> target2{ REL::RelocationID(75461, 77246, 0xDBBDD),
            REL::VariantOffset(0x9, 0x9, 0x15) };
        SKSE::stl::write_thunk_call<DXGIPresentHook>(target2);
        SKSE::log::info("Hooked 0x{:016X} call.", target2.address());
    }
}
