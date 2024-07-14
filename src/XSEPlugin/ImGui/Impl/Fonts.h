#pragma once

#include <imgui.h>

namespace ImGui::Impl
{
    class Fonts
    {
    public:
        /// Fetch and apply the latest fonts.
        ///
        /// @note
        ///   Assume caller has already acquired shared lock of
        ///   configuration and translation before calling.
        void Load();

        /// Update glyph ranges.
        void Feed(std::string_view a_text);

        /// Rebuild fonts if glyph ranges change.
        void Refresh();

    private:
        /// Rebuild fonts from current config and glyph ranges.
        void Rebuild();

        std::string _path;
        float       _size;

        ImFontConfig             _config;
        ImFontGlyphRangesBuilder _rangesBuilder;

        bool _wantRefresh{ false };
    };
}
