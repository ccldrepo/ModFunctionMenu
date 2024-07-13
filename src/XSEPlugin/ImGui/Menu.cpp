#include "Menu.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <XSEPlugin/Base/StringTable.h>
#include <XSEPlugin/ImGui/Renderer.h>

namespace ImGui
{
    void Menu::Open()
    {
        Renderer::Enable();
        _isOpen.store(true);
        SKSE::log::debug("Open menu.");
    }

    void Menu::Close()
    {
        _isOpen.store(false);
        SKSE::log::debug("Close menu.");
    }

    void Menu::Draw()
    {
        auto stbl = StringTable::GetSingleton();
        stbl->CheckReload();

        auto datastore = Datastore::GetSingleton();

        auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });
        ImGui::SetNextWindowSize(ImVec2{ viewport->Size.x * 0.3f, viewport->Size.y * 0.5f }, ImGuiCond_Appearing);

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoCollapse;

        ImGui::Begin(stbl->Title.c_str(), nullptr, window_flags);
        {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            if (ImGui::BeginTabBar("TabBar", tab_bar_flags)) {
                if (ImGui::BeginTabItem(stbl->Section_Mod.c_str())) {
                    datastore->CurrentSection(datastore->modTree);
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem(stbl->Section_Config.c_str())) {
                    datastore->CurrentSection(datastore->configTree);
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            DrawExplorer(datastore);
        }
        ImGui::End();

#ifdef _DEBUG
        ImGui::ShowDemoWindow();
#endif
    }

    void Menu::DrawExplorer(Datastore* datastore)
    {
        auto tree = datastore->CurrentSection();
        ImGui::Text("%s", tree->CurrentPathStr().c_str());
        ImGui::Spacing();

        if (ImGui::BeginTable("Explorer", 1)) {
            ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2{ 0.0f, 0.0f });

            ImVec2 sz = ImVec2(-FLT_MIN, 0.0f);

            ImGui::TableNextColumn();
            if (ImGui::Button("..", sz)) {
                OnClickParentEntry(tree);
            }

            auto node = tree->CurrentPath();
            for (auto& entry : node->children) {
                ImGui::TableNextColumn();
                Renderer::fonts.Feed(entry->name);
                if (ImGui::Button(entry->name.c_str(), sz)) {
                    OnClickEntry(tree, entry.get());
                }
            }

            DrawMessageBox(datastore);

            ImGui::PopStyleVar();
            ImGui::EndTable();
        }
    }

    void Menu::DrawMessageBox(Datastore* datastore)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("MessageBox", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("%s", _msg.data());
            ImGui::Spacing();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
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

                InvokeFunction(func);

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

    void Menu::InvokeFunction(const MFM_Function& a_func)
    {
        switch (a_func.type) {
        case MFMAPI_Type::kVoid:
            SKSE::log::debug("Invoke Void function.");
            a_func();
            break;
        case MFMAPI_Type::kMessage:
            SKSE::log::debug("Invoke Message function.");
            // TODO
        case MFMAPI_Type::kMessageBox:
            SKSE::log::debug("Invoke MessageBox function.");
            {
                _msg.resize(0x4000);
                a_func(_msg.data(), _msg.size());
                _msg.back() = '\0';

                ImGui::OpenPopup("MessageBox");
            }
            break;
        }
    }
}
