#include "Menu.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "../Application.h"
#include "../InputManager.h"
#include "../Util/TOML.h"
#include "../Util/Win.h"
#include "Renderer.h"

void Invoke(Menu* menu, MFMTree* tree, const MFMNode* node)
{
    switch (node->type) {
    case MFMNode::Type::kRegular:
        {
            auto        data = LoadTOMLFile(node->path);
            std::string dll;
            std::string api;
            LoadTOMLValue(data, "dll"sv, dll);
            LoadTOMLValue(data, "api"sv, api);
            auto dll_path = StrToPath(dll);
            auto func = GetModuleFunc<void (*)()>(dll_path.c_str(), api.c_str());
            func();
            menu->Close();
        }
        break;
    case MFMNode::Type::kDirectory:
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
