#include "Fonts.h"

#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>

#include <XSEPlugin/Base/Configuration.h>
#include <XSEPlugin/Base/Translation.h>

namespace ImGui::Impl
{
    void Fonts::Load()
    {
        auto& cfgFonts = Configuration::GetSingleton()->fonts;

        _path = cfgFonts.general.sFont;
        _size = cfgFonts.general.fSize;

        _config = ImFontConfig();
        _rangesBuilder = ImFontGlyphRangesBuilder();

        _wantRefresh = false;

        // Feed default glyph ranges.
        _rangesBuilder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesDefault());
        Translation::GetSingleton()->Visit(  //
            [this]([[maybe_unused]] const std::string& key, const std::string& value) {
                _rangesBuilder.AddText(value.c_str());
            });

        Rebuild();
    }

    void Fonts::Feed(std::string_view a_text)
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

            if (!_rangesBuilder.GetBit(c)) {
                _rangesBuilder.SetBit(c);
                _wantRefresh = true;
            }
        }
    }

    void Fonts::Refresh()
    {
        if (!_wantRefresh) {
            return;
        }

        _wantRefresh = false;

        Rebuild();
        SKSE::log::trace("Refresh font.");
    }

    void Fonts::Rebuild()
    {
        auto& io = ImGui::GetIO();
        io.Fonts->Clear();

        ImVector<ImWchar> ranges;
        _rangesBuilder.BuildRanges(&ranges);

        io.Fonts->AddFontFromFileTTF(_path.c_str(), _size, &_config, ranges.Data);
        io.Fonts->Build();

        ImGui_ImplDX11_InvalidateDeviceObjects();
        ImGui_ImplDX11_CreateDeviceObjects();
    }
}
