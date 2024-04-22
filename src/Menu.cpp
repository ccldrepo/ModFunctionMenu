#include "Menu.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

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

void Menu::Draw() { ImGui::ShowDemoWindow(); }
