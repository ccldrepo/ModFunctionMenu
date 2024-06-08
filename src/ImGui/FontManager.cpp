#include "FontManager.h"

#include <absl/cleanup/cleanup.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "../Configuration.h"

namespace ImGui
{
    void FontManager::Init(bool a_abort)
    {
        auto tmp = std::unique_ptr<FontManager>{ new FontManager };
        tmp->Load(a_abort);
        _singleton = std::move(tmp);
    }

    void FontManager::Load(bool a_abort)
    {
        auto config = Configuration::GetSingleton();
        try {
            if (config->general.sFont.empty()) {
                auto& io = ImGui::GetIO();
                io.Fonts->Clear();
                io.Fonts->AddFontDefault();
                io.Fonts->Build();
                SKSE::log::info("Successfully loaded font from ImGui's default.");
            } else {
                LoadImpl(config->general.sFont.c_str());
                SKSE::log::info("Successfully loaded font from \"{}\".", config->general.sFont);
            }
            ImGui_ImplDX11_InvalidateDeviceObjects();
            ImGui_ImplDX11_CreateDeviceObjects();
        } catch (const std::exception& e) {
            auto msg = std::format("Failed to load font from \"{}\": {}.", config->general.sFont, e.what());
            SKSE::stl::report_fatal_error(msg, a_abort);
        }
    }

    void FontManager::LoadImpl(const char* a_path)
    {
        auto& io = ImGui::GetIO();
        io.Fonts->Clear();

        // Feed default ranges.
        rangesBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());

        ImVector<ImWchar> ranges;
        rangesBuilder.BuildRanges(&ranges);

        auto font = io.Fonts->AddFontFromFileTTF(a_path, 20, &fontConfig, ranges.Data);
        io.Fonts->Build();
    }

    void FontManager::Feed(std::string_view a_text)
    {
        if (useDefault) {
            return;
        }

        auto text = a_text.data();
        auto text_end = text + a_text.size();
        while (text < text_end) {
            unsigned int c = 0;
            int          c_len = ImTextCharFromUtf8(&c, text, text_end);
            text += c_len;
            if (c_len == 0) {
                break;
            }
            if (!rangesBuilder.GetBit(c)) {
                rangesBuilder.SetBit(c);
                needRefresh = true;
            }
        }
    }

    void FontManager::Refresh()
    {
        if (!needRefresh) {
            return;
        }

        auto cleanup = [this]() { needRefresh = false; };

        auto& io = ImGui::GetIO();
        io.Fonts->Clear();

        ImVector<ImWchar> ranges;
        rangesBuilder.BuildRanges(&ranges);

        auto config = Configuration::GetSingleton();
        io.Fonts->AddFontFromFileTTF(config->general.sFont.c_str(), 20, &fontConfig, ranges.Data);
        io.Fonts->Build();

        ImGui_ImplDX11_InvalidateDeviceObjects();
        ImGui_ImplDX11_CreateDeviceObjects();
    }
}
