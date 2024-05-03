#include "Menu.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "../Application.h"
#include "../Core.h"
#include "../InputManager.h"
#include "Renderer.h"

void Invoke(Menu* menu, MFM_Tree* tree, const MFM_Node* node)
{
    switch (node->type) {
    case MFM_Node::Type::kRegular:
        {
            auto func = MFM_Function::Get(node->path);
            func();
        }
        break;
    case MFM_Node::Type::kDirectory:
        {
            tree->CurrentRoot(node);
        }
        break;
    }
}

void Menu::Open()
{
    //auto uiMQ = RE::UIMessageQueue::GetSingleton();
    //uiMQ->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kShow, nullptr);
    isOpen = true;
    ImGui::shouldRender = true;
    InputManager::SetBlocked();
}

void Menu::Close()
{
    //auto uiMQ = RE::UIMessageQueue::GetSingleton();
    //uiMQ->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
    isOpen = false;
    ImGui::shouldRender = false;
    InputManager::SetWantUnblock();
}

void Menu::Draw()
{
    auto app = Application::GetSingleton();

    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2{ 0.5f, 0.5f });
    ImGui::SetNextWindowSize(ImVec2{ viewport->Size.x * 0.5f, viewport->Size.y * 0.8f });

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Mod Function Menu");
    {
        auto& tree = app->modTree;
        ImGui::Text("%s", tree.CurrentRootStr().c_str());

        if (ImGui::BeginTable("Explorer", 1)) {
            auto currentRoot = tree.CurrentRoot();
            for (auto& entry : currentRoot->children) {
                ImGui::TableNextColumn();
                if (ImGui::Button(entry.name.c_str())) {
                    Invoke(this, &tree, &entry);
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
}
