#include "Menu.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "Application.h"
#include "ImGui/Renderer.h"
#include "InputManager.h"

void Menu::Open()
{
    isOpen = true;
    ImGui::shouldRender = true;
    InputManager::SetBlocked();
}

void Menu::Close()
{
    isOpen = false;
    ImGui::shouldRender = false;
    InputManager::SetWantUnblock();
}

void Menu::Toggle()
{
    if (!IsOpen()) {
        Open();
    } else {
        Close();
    }
}

void Menu::Draw()
{
    auto app = Application::GetSingleton();

    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2{ 0.5f, 0.5f });
    ImGui::SetNextWindowSize(ImVec2{ viewport->Size.x * 0.5f, viewport->Size.y * 0.5f });

    ImGui::Begin("Mod Function Menu");
    {
        auto& root = app->modRoot;
        ImGui::Text("%s", root.CurrentRootStr().c_str());
    }
    ImGui::End();
    //ImGui::ShowDemoWindow();
}
