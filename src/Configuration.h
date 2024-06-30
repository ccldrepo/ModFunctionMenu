#pragma once

#include "Util/Singleton.h"

class Configuration final : public SingletonEx<Configuration>
{
    friend class SingletonEx<Configuration>;

public:
    // Initialize or reload configuration.
    static void Init(bool a_abort = true);

    struct General
    {
        std::string sLanguage;
        std::string sFont;
    };

    struct Controls
    {
        struct Keyboard
        {
            std::uint32_t iHotkey{ REX::W32::DIK_F1 };
            std::uint32_t iModifier{ 0 };
            std::uint32_t iExtraExit{ REX::W32::DIK_ESCAPE };
        };

        struct Gamepad
        {
            std::uint32_t iHotkey{ 0 };
            std::uint32_t iModifier{ 0 };
            std::uint32_t iExtraExit{ 0 };
        };

        Keyboard keyboard;
        Gamepad  gamepad;
    };

    struct Styles
    {
        struct Colors
        {
            std::uint32_t iText;
            std::uint32_t iTextDisabled;
            std::uint32_t iWindowBg;
            std::uint32_t iChildBg;
            std::uint32_t iPopupBg;
            std::uint32_t iBorder;
            std::uint32_t iBorderShadow;
            std::uint32_t iFrameBg;
            std::uint32_t iFrameBgHovered;
            std::uint32_t iFrameBgActive;
            std::uint32_t iTitleBg;
            std::uint32_t iTitleBgActive;
            std::uint32_t iTitleBgCollapsed;
            std::uint32_t iMenuBarBg;
            std::uint32_t iScrollbarBg;
            std::uint32_t iScrollbarGrab;
            std::uint32_t iScrollbarGrabHovered;
            std::uint32_t iScrollbarGrabActive;
            std::uint32_t iCheckMark;
            std::uint32_t iSliderGrab;
            std::uint32_t iSliderGrabActive;
            std::uint32_t iButton;
            std::uint32_t iButtonHovered;
            std::uint32_t iButtonActive;
            std::uint32_t iHeader;
            std::uint32_t iHeaderHovered;
            std::uint32_t iHeaderActive;
            std::uint32_t iSeparator;
            std::uint32_t iSeparatorHovered;
            std::uint32_t iSeparatorActive;
            std::uint32_t iResizeGrip;
            std::uint32_t iResizeGripHovered;
            std::uint32_t iResizeGripActive;
            std::uint32_t iTab;
            std::uint32_t iTabHovered;
            std::uint32_t iTabActive;
            std::uint32_t iTabUnfocused;
            std::uint32_t iTabUnfocusedActive;
            std::uint32_t iPlotLines;
            std::uint32_t iPlotLinesHovered;
            std::uint32_t iPlotHistogram;
            std::uint32_t iPlotHistogramHovered;
            std::uint32_t iTableHeaderBg;
            std::uint32_t iTableBorderStrong;
            std::uint32_t iTableBorderLight;
            std::uint32_t iTableRowBg;
            std::uint32_t iTableRowBgAlt;
            std::uint32_t iTextSelectedBg;
            std::uint32_t iDragDropTarget;
            std::uint32_t iNavHighlight;
            std::uint32_t iNavWindowingHighlight;
            std::uint32_t iNavWindowingDimBg;
            std::uint32_t iModalWindowDimBg;
        };

        Colors colors;
    };

    General  general;
    Controls controls;
    Styles   styles;

private:
    Configuration() = default;

    ~Configuration() = default;

private:
    using LoadImplFunc = void (Configuration::*)(const std::filesystem::path&);
    using SaveImplFunc = void (Configuration::*)(const std::filesystem::path&) const;

    void Load(LoadImplFunc a_func, const std::filesystem::path& a_path, bool a_abort);
    void Save(SaveImplFunc a_func, const std::filesystem::path& a_path, bool a_abort) const;

    void LoadImpl(const std::filesystem::path& a_path);
    void SaveImpl(const std::filesystem::path& a_path) const;

    void LoadImpl_Styles(const std::filesystem::path& a_path);
    void SaveImpl_Styles(const std::filesystem::path& a_path) const;

    static inline const std::filesystem::path _path{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu.toml"sv };
    static inline const std::filesystem::path _path_styles{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu_styles.toml"sv };
};
