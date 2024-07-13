#pragma once

#include <imgui.h>

namespace ImGui::Impl
{
    class Fonts
    {
    public:
        // Initialize or reload font.
        void Load();

        // Dynamically collect text and rebuild font atlas.
        void Feed(std::string_view a_text);
        void Refresh();

    private:
        // Rebuild font from current config and glyph ranges.
        void Rebuild();

        std::string _path;
        float       _size;

        ImFontConfig             _config;
        ImFontGlyphRangesBuilder _rangesBuilder;

        bool _wantRefresh{ false };
    };
}
