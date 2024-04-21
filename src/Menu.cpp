#include "Menu.h"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "ImGui/Renderer.h"

void Menu::Open()
{
    isOpen = true;
    ImGui::shouldRender = true;
}

void Menu::Close()
{
    isOpen = false;
    ImGui::shouldRender = false;
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
