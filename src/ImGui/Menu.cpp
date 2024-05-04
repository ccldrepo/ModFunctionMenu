#include "Menu.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "../Application.h"
#include "../Core.h"
#include "../InputManager.h"
#include "Renderer.h"

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
    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2{ 0.5f, 0.5f });
    ImGui::SetNextWindowSize(ImVec2{ viewport->Size.x * 0.5f, viewport->Size.y * 0.6f });

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_MenuBar;
    ImGui::Begin("Mod Function Menu");
    {
        auto app = Application::GetSingleton();

        auto tree = app->CurrentSection();
        ImGui::Text("%s", tree->CurrentPathStr().c_str());

        if (ImGui::BeginTable("Explorer", 1)) {
            ImGui::TableNextColumn();
            if (ImGui::Button("..")) {
                OnClickParentEntry(tree);
            }

            auto node = tree->CurrentPath();
            for (auto& entry : node->children) {
                ImGui::TableNextColumn();
                if (ImGui::Button(entry.name.c_str())) {
                    OnClickEntry(tree, std::addressof(entry));
                }
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();

    ImGui::ShowDemoWindow();
}

void Menu::OnClickParentEntry(MFM_Tree* a_tree) { a_tree->ResetCurrentPathToParent(); }

void Menu::OnClickEntry(MFM_Tree* a_tree, const MFM_Node* a_node)
{
    switch (a_node->type) {
    case MFM_Node::Type::kRegular:
        {
            auto func = MFM_Function::Get(a_node->path);

            switch (func.preAction) {
            case MFMAPI_PreAction::kNone:
                break;
            case MFMAPI_PreAction::kCloseMenu:
                Close();
                break;
            case MFMAPI_PreAction::kCloseMenuAndResetPath:
                Close();
                a_tree->ResetCurrentPath();
                break;
            }

            switch (func.type) {
            case MFMAPI_Type::kVoid:
                func();
                break;
            case MFMAPI_Type::kMessage:
                // TODO
                break;
            case MFMAPI_Type::kMessageBox:
                // TODO
                break;
            }

            switch (func.postAction) {
            case MFMAPI_PostAction::kNone:
                break;
            case MFMAPI_PostAction::kCloseMenu:
                Close();
                break;
            case MFMAPI_PostAction::kCloseMenuAndResetPath:
                Close();
                a_tree->ResetCurrentPath();
                break;
            }
        }
        break;
    case MFM_Node::Type::kDirectory:
        {
            a_tree->CurrentPath(a_node);
        }
        break;
    }
}
