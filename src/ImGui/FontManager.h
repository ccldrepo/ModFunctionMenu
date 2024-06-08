#pragma once

#include <imgui.h>

namespace ImGui
{
    class FontManager
    {
    public:
        [[nodiscard]] static FontManager* GetSingleton() { return _singleton.get(); }

        FontManager(const FontManager&) = delete;
        FontManager(FontManager&&) = delete;
        FontManager& operator=(const FontManager&) = delete;
        FontManager& operator=(FontManager&&) = delete;

        // Initialize or reload font.
        static void Init(bool a_abort = true);

        static bool IsWantReload() noexcept { return wantReload.load(); }

        static void SetWantReload() noexcept { wantReload.store(true); }

        static void HandleWantReload()
        {
            wantReload.store(false);
            Init(false);
        }

        // Dynamically collect text and rebuild font atlas.
        void Feed(std::string_view a_text);
        void Refresh();

    private:
        FontManager() = default;

        void Load(bool a_abort);
        void LoadImpl(const char* a_path);

        static inline std::unique_ptr<FontManager> _singleton;

        static inline std::atomic<bool> wantReload{ false };

        ImFontConfig             fontConfig;
        ImFontGlyphRangesBuilder rangesBuilder;
        bool                     useDefault{ false };
        bool                     wantRefresh{ false };
    };
}
