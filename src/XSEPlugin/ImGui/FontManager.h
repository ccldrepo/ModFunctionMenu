#pragma once

#include <imgui.h>

#include <XSEPlugin/Util/Singleton.h>

namespace ImGui
{
    class FontManager final : public SingletonEx<FontManager>
    {
        friend class SingletonEx<FontManager>;

    public:
        // Initialize or reload font.
        static void Init(bool a_abort = true);

        // Dynamically collect text and rebuild font atlas.
        void Feed(std::string_view a_text);
        void Refresh();

    private:
        FontManager() = default;

        ~FontManager() = default;

    private:
        void Load(bool a_abort);
        void LoadImpl(const char* a_path);

        ImFontConfig             fontConfig;
        ImFontGlyphRangesBuilder rangesBuilder;

        bool _wantRefresh{ false };
    };
}
