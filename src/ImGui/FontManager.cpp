#include "FontManager.h"

#include <absl/cleanup/cleanup.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include "../Configuration.h"
#include "../Translation.h"

namespace ImGui
{
    void FontManager::Init(bool a_abort)
    {
        auto tmp = std::unique_ptr<FontManager, Deleter>{ new FontManager };
        tmp->Load(a_abort);
        _singleton = std::move(tmp);
    }

    void FontManager::Load(bool a_abort)
    {
        auto& cfgFonts = Configuration::GetSingleton()->fonts;
        try {
            LoadImpl(cfgFonts.general.sFont.c_str());
            SKSE::log::info("Successfully loaded font from \"{}\".", cfgFonts.general.sFont);
        } catch (const std::system_error& e) {
            auto msg = std::format("Failed to load font from \"{}\": {}.", cfgFonts.general.sFont,
                SKSE::stl::ansi_to_utf8(e.what()).value_or(e.what()));
            SKSE::stl::report_fatal_error(msg, a_abort);
        } catch (const std::exception& e) {
            auto msg = std::format("Failed to load font from \"{}\": {}.", cfgFonts.general.sFont, e.what());
            SKSE::stl::report_fatal_error(msg, a_abort);
        }
    }

    void FontManager::LoadImpl(const char* a_path)
    {
        auto st = std::filesystem::status(StrToPath(a_path));

        if (!std::filesystem::exists(st)) {
            throw std::runtime_error("File does not exist");
        }

        if (!std::filesystem::is_regular_file(st)) {
            throw std::runtime_error("File is not a regular file");
        }

        auto& cfgFonts = Configuration::GetSingleton()->fonts;

        auto& io = ImGui::GetIO();
        io.Fonts->Clear();

        // Feed default ranges.
        rangesBuilder.AddRanges(io.Fonts->GetGlyphRangesDefault());
        Translation::GetSingleton()->Visit(  //
            [this]([[maybe_unused]] const std::string& key, const std::string& value) {
                rangesBuilder.AddText(value.c_str());
            });

        ImVector<ImWchar> ranges;
        rangesBuilder.BuildRanges(&ranges);

        auto font = io.Fonts->AddFontFromFileTTF(a_path, cfgFonts.general.fSize, &fontConfig, ranges.Data);
        io.Fonts->Build();

        ImGui_ImplDX11_InvalidateDeviceObjects();
        ImGui_ImplDX11_CreateDeviceObjects();
    }

    void FontManager::Feed(std::string_view a_text)
    {
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
                _wantRefresh = true;
            }
        }
    }

    void FontManager::Refresh()
    {
        if (!_wantRefresh) {
            return;
        }

        absl::Cleanup cleanup = [this]() { _wantRefresh = false; };

        auto& io = ImGui::GetIO();
        io.Fonts->Clear();

        ImVector<ImWchar> ranges;
        rangesBuilder.BuildRanges(&ranges);

        auto& cfgFonts = Configuration::GetSingleton()->fonts;
        io.Fonts->AddFontFromFileTTF(cfgFonts.general.sFont.c_str(), cfgFonts.general.fSize, &fontConfig, ranges.Data);
        io.Fonts->Build();

        ImGui_ImplDX11_InvalidateDeviceObjects();
        ImGui_ImplDX11_CreateDeviceObjects();

        SKSE::log::debug("Refresh font.");
    }
}
