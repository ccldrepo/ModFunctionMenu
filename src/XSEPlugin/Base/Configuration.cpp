#include "Configuration.h"

#include <toml++/toml.hpp>

#include <XSEPlugin/Util/File.h>
#include <XSEPlugin/Util/TOML.h>

void Configuration::Init(bool a_abort)
{
    auto tmp = std::unique_ptr<Configuration, Deleter>{ new Configuration };

    if (std::filesystem::exists(_path)) {
        tmp->Load(&Configuration::LoadImpl, _path, a_abort);
    } else {
        // Export default config if config file not exists.
        tmp->Save(&Configuration::SaveImpl, _path, a_abort);
    }

    if (std::filesystem::exists(_path_fonts)) {
        tmp->Load(&Configuration::LoadImpl_Fonts, _path_fonts, a_abort);
    } else {
        // Export default config if config file not exists.
        tmp->Save(&Configuration::SaveImpl_Fonts, _path_fonts, a_abort);
    }

    if (std::filesystem::exists(_path_styles)) {
        tmp->Load(&Configuration::LoadImpl_Styles, _path_styles, a_abort);
    } else {
        // Export default config if config file not exists.
        tmp->Save(&Configuration::SaveImpl_Styles, _path_styles, a_abort);
    }

    // Assume the caller has already acquired the lock.
    _singleton = std::move(tmp);
    // Assume the caller will increase version.
}

void Configuration::Load(LoadImplFunc a_func, const std::filesystem::path& a_path, bool a_abort)
{
    try {
        (this->*a_func)(a_path);
        SKSE::log::info("Successfully loaded configuration from \"{}\".", PathToStr(a_path));
    } catch (const toml::parse_error& e) {
        auto msg = std::format("Failed to load configuration from \"{}\" (error occurred at line {}, column {}): {}.",
            PathToStr(a_path), e.source().begin.line, e.source().begin.column, e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    } catch (const std::system_error& e) {
        auto msg = std::format("Failed to load configuration from \"{}\": {}.", PathToStr(a_path),
            SKSE::stl::ansi_to_utf8(e.what()).value_or(e.what()));
        SKSE::stl::report_fatal_error(msg, a_abort);
    } catch (const std::exception& e) {
        auto msg = std::format("Failed to load configuration from \"{}\": {}.", PathToStr(a_path), e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    }
}

void Configuration::Save(SaveImplFunc a_func, const std::filesystem::path& a_path, bool a_abort) const
{
    try {
        (this->*a_func)(a_path);
        SKSE::log::info("Successfully saved configuration to \"{}\".", PathToStr(a_path));
    } catch (const std::system_error& e) {
        auto msg = std::format("Failed to save configuration to \"{}\": {}.", PathToStr(a_path),
            SKSE::stl::ansi_to_utf8(e.what()).value_or(e.what()));
        SKSE::stl::report_fatal_error(msg, a_abort);
    } catch (const std::exception& e) {
        auto msg = std::format("Failed to save configuration to \"{}\": {}.", PathToStr(a_path), e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    }
}

void Configuration::LoadImpl(const std::filesystem::path& a_path)
{
    auto data = LoadTOMLFile(a_path);

    if (auto section = GetTOMLSection(data, "General"sv)) {
        GetTOMLValue(*section, "sLanguage"sv, general.sLanguage);
    }

    if (auto section = GetTOMLSection(data, "Controls"sv)) {
        if (auto subsection = GetTOMLSection(*section, "Keyboard"sv)) {
            GetTOMLValue(*subsection, "iHotkey"sv, controls.keyboard.iHotkey);
            GetTOMLValue(*subsection, "iModifier"sv, controls.keyboard.iModifier);
            GetTOMLValue(*subsection, "iExtraExit"sv, controls.keyboard.iExtraExit);
        }

        if (auto subsection = GetTOMLSection(*section, "Gamepad"sv)) {
            GetTOMLValue(*subsection, "iHotkey"sv, controls.gamepad.iHotkey);
            GetTOMLValue(*subsection, "iModifier"sv, controls.gamepad.iModifier);
            GetTOMLValue(*subsection, "iExtraExit"sv, controls.gamepad.iExtraExit);
        }
    }
}

void Configuration::SaveImpl(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        SetTOMLValue(section, "sLanguage"sv, general.sLanguage);
        SetTOMLSection(data, "General"sv, std::move(section));
    }
    {
        toml::table section;
        {
            toml::table subsection;
            SetTOMLValue(subsection, "iHotkey"sv, controls.keyboard.iHotkey);
            SetTOMLValue(subsection, "iModifier"sv, controls.keyboard.iModifier);
            SetTOMLValue(subsection, "iExtraExit"sv, controls.keyboard.iExtraExit);
            SetTOMLSection(section, "Keyboard"sv, std::move(subsection));
        }
        {
            toml::table subsection;
            SetTOMLValue(subsection, "iHotkey"sv, controls.gamepad.iHotkey);
            SetTOMLValue(subsection, "iModifier"sv, controls.gamepad.iModifier);
            SetTOMLValue(subsection, "iExtraExit"sv, controls.gamepad.iExtraExit);
            SetTOMLSection(section, "Gamepad"sv, std::move(subsection));
        }
        SetTOMLSection(data, "Controls"sv, std::move(section));
    }
    SaveTOMLFile(a_path, data);
}

void Configuration::LoadImpl_Fonts(const std::filesystem::path& a_path)
{
    auto data = LoadTOMLFile(a_path);

    if (auto section = GetTOMLSection(data, "General"sv)) {
        GetTOMLValue(*section, "sFont"sv, fonts.general.sFont);
        CheckRegularFile(StrToPath(fonts.general.sFont));
        GetTOMLValue(*section, "fSize"sv, fonts.general.fSize);
    }
}

void Configuration::SaveImpl_Fonts(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        SetTOMLValue(section, "sFont"sv, fonts.general.sFont);
        SetTOMLValue(section, "fSize"sv, fonts.general.fSize);
        SetTOMLSection(data, "General"sv, std::move(section));
    }
    SaveTOMLFile(a_path, data);
}

void Configuration::LoadImpl_Styles(const std::filesystem::path& a_path)
{
    auto data = LoadTOMLFile(a_path);

    if (auto section = GetTOMLSection(data, "Colors"sv)) {
        GetTOMLValue(*section, "iText"sv, styles.colors.iText);
        GetTOMLValue(*section, "iTextDisabled"sv, styles.colors.iTextDisabled);
        GetTOMLValue(*section, "iWindowBg"sv, styles.colors.iWindowBg);
        GetTOMLValue(*section, "iChildBg"sv, styles.colors.iChildBg);
        GetTOMLValue(*section, "iPopupBg"sv, styles.colors.iPopupBg);
        GetTOMLValue(*section, "iBorder"sv, styles.colors.iBorder);
        GetTOMLValue(*section, "iBorderShadow"sv, styles.colors.iBorderShadow);
        GetTOMLValue(*section, "iFrameBg"sv, styles.colors.iFrameBg);
        GetTOMLValue(*section, "iFrameBgHovered"sv, styles.colors.iFrameBgHovered);
        GetTOMLValue(*section, "iFrameBgActive"sv, styles.colors.iFrameBgActive);
        GetTOMLValue(*section, "iTitleBg"sv, styles.colors.iTitleBg);
        GetTOMLValue(*section, "iTitleBgActive"sv, styles.colors.iTitleBgActive);
        GetTOMLValue(*section, "iTitleBgCollapsed"sv, styles.colors.iTitleBgCollapsed);
        GetTOMLValue(*section, "iMenuBarBg"sv, styles.colors.iMenuBarBg);
        GetTOMLValue(*section, "iScrollbarBg"sv, styles.colors.iScrollbarBg);
        GetTOMLValue(*section, "iScrollbarGrab"sv, styles.colors.iScrollbarGrab);
        GetTOMLValue(*section, "iScrollbarGrabHovered"sv, styles.colors.iScrollbarGrabHovered);
        GetTOMLValue(*section, "iScrollbarGrabActive"sv, styles.colors.iScrollbarGrabActive);
        GetTOMLValue(*section, "iCheckMark"sv, styles.colors.iCheckMark);
        GetTOMLValue(*section, "iSliderGrab"sv, styles.colors.iSliderGrab);
        GetTOMLValue(*section, "iSliderGrabActive"sv, styles.colors.iSliderGrabActive);
        GetTOMLValue(*section, "iButton"sv, styles.colors.iButton);
        GetTOMLValue(*section, "iButtonHovered"sv, styles.colors.iButtonHovered);
        GetTOMLValue(*section, "iButtonActive"sv, styles.colors.iButtonActive);
        GetTOMLValue(*section, "iHeader"sv, styles.colors.iHeader);
        GetTOMLValue(*section, "iHeaderHovered"sv, styles.colors.iHeaderHovered);
        GetTOMLValue(*section, "iHeaderActive"sv, styles.colors.iHeaderActive);
        GetTOMLValue(*section, "iSeparator"sv, styles.colors.iSeparator);
        GetTOMLValue(*section, "iSeparatorHovered"sv, styles.colors.iSeparatorHovered);
        GetTOMLValue(*section, "iSeparatorActive"sv, styles.colors.iSeparatorActive);
        GetTOMLValue(*section, "iResizeGrip"sv, styles.colors.iResizeGrip);
        GetTOMLValue(*section, "iResizeGripHovered"sv, styles.colors.iResizeGripHovered);
        GetTOMLValue(*section, "iResizeGripActive"sv, styles.colors.iResizeGripActive);
        GetTOMLValue(*section, "iTabHovered"sv, styles.colors.iTabHovered);
        GetTOMLValue(*section, "iTab"sv, styles.colors.iTab);
        GetTOMLValue(*section, "iTabSelected"sv, styles.colors.iTabSelected);
        GetTOMLValue(*section, "iTabSelectedOverline"sv, styles.colors.iTabSelectedOverline);
        GetTOMLValue(*section, "iTabDimmed"sv, styles.colors.iTabDimmed);
        GetTOMLValue(*section, "iTabDimmedSelected"sv, styles.colors.iTabDimmedSelected);
        GetTOMLValue(*section, "iTabDimmedSelectedOverline"sv, styles.colors.iTabDimmedSelectedOverline);
        GetTOMLValue(*section, "iTabUnfocused"sv, styles.colors.iTabUnfocused);
        GetTOMLValue(*section, "iTabUnfocusedActive"sv, styles.colors.iTabUnfocusedActive);
        GetTOMLValue(*section, "iPlotLines"sv, styles.colors.iPlotLines);
        GetTOMLValue(*section, "iPlotLinesHovered"sv, styles.colors.iPlotLinesHovered);
        GetTOMLValue(*section, "iPlotHistogram"sv, styles.colors.iPlotHistogram);
        GetTOMLValue(*section, "iPlotHistogramHovered"sv, styles.colors.iPlotHistogramHovered);
        GetTOMLValue(*section, "iTableHeaderBg"sv, styles.colors.iTableHeaderBg);
        GetTOMLValue(*section, "iTableBorderStrong"sv, styles.colors.iTableBorderStrong);
        GetTOMLValue(*section, "iTableBorderLight"sv, styles.colors.iTableBorderLight);
        GetTOMLValue(*section, "iTableRowBg"sv, styles.colors.iTableRowBg);
        GetTOMLValue(*section, "iTableRowBgAlt"sv, styles.colors.iTableRowBgAlt);
        GetTOMLValue(*section, "iTextSelectedBg"sv, styles.colors.iTextSelectedBg);
        GetTOMLValue(*section, "iDragDropTarget"sv, styles.colors.iDragDropTarget);
        GetTOMLValue(*section, "iNavHighlight"sv, styles.colors.iNavHighlight);
        GetTOMLValue(*section, "iNavWindowingHighlight"sv, styles.colors.iNavWindowingHighlight);
        GetTOMLValue(*section, "iNavWindowingDimBg"sv, styles.colors.iNavWindowingDimBg);
        GetTOMLValue(*section, "iModalWindowDimBg"sv, styles.colors.iModalWindowDimBg);
    }
}

void Configuration::SaveImpl_Styles(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        SetTOMLValue(section, "iText"sv, styles.colors.iText);
        SetTOMLValue(section, "iTextDisabled"sv, styles.colors.iTextDisabled);
        SetTOMLValue(section, "iWindowBg"sv, styles.colors.iWindowBg);
        SetTOMLValue(section, "iChildBg"sv, styles.colors.iChildBg);
        SetTOMLValue(section, "iPopupBg"sv, styles.colors.iPopupBg);
        SetTOMLValue(section, "iBorder"sv, styles.colors.iBorder);
        SetTOMLValue(section, "iBorderShadow"sv, styles.colors.iBorderShadow);
        SetTOMLValue(section, "iFrameBg"sv, styles.colors.iFrameBg);
        SetTOMLValue(section, "iFrameBgHovered"sv, styles.colors.iFrameBgHovered);
        SetTOMLValue(section, "iFrameBgActive"sv, styles.colors.iFrameBgActive);
        SetTOMLValue(section, "iTitleBg"sv, styles.colors.iTitleBg);
        SetTOMLValue(section, "iTitleBgActive"sv, styles.colors.iTitleBgActive);
        SetTOMLValue(section, "iTitleBgCollapsed"sv, styles.colors.iTitleBgCollapsed);
        SetTOMLValue(section, "iMenuBarBg"sv, styles.colors.iMenuBarBg);
        SetTOMLValue(section, "iScrollbarBg"sv, styles.colors.iScrollbarBg);
        SetTOMLValue(section, "iScrollbarGrab"sv, styles.colors.iScrollbarGrab);
        SetTOMLValue(section, "iScrollbarGrabHovered"sv, styles.colors.iScrollbarGrabHovered);
        SetTOMLValue(section, "iScrollbarGrabActive"sv, styles.colors.iScrollbarGrabActive);
        SetTOMLValue(section, "iCheckMark"sv, styles.colors.iCheckMark);
        SetTOMLValue(section, "iSliderGrab"sv, styles.colors.iSliderGrab);
        SetTOMLValue(section, "iSliderGrabActive"sv, styles.colors.iSliderGrabActive);
        SetTOMLValue(section, "iButton"sv, styles.colors.iButton);
        SetTOMLValue(section, "iButtonHovered"sv, styles.colors.iButtonHovered);
        SetTOMLValue(section, "iButtonActive"sv, styles.colors.iButtonActive);
        SetTOMLValue(section, "iHeader"sv, styles.colors.iHeader);
        SetTOMLValue(section, "iHeaderHovered"sv, styles.colors.iHeaderHovered);
        SetTOMLValue(section, "iHeaderActive"sv, styles.colors.iHeaderActive);
        SetTOMLValue(section, "iSeparator"sv, styles.colors.iSeparator);
        SetTOMLValue(section, "iSeparatorHovered"sv, styles.colors.iSeparatorHovered);
        SetTOMLValue(section, "iSeparatorActive"sv, styles.colors.iSeparatorActive);
        SetTOMLValue(section, "iResizeGrip"sv, styles.colors.iResizeGrip);
        SetTOMLValue(section, "iResizeGripHovered"sv, styles.colors.iResizeGripHovered);
        SetTOMLValue(section, "iResizeGripActive"sv, styles.colors.iResizeGripActive);
        SetTOMLValue(section, "iTabHovered"sv, styles.colors.iTabHovered);
        SetTOMLValue(section, "iTab"sv, styles.colors.iTab);
        SetTOMLValue(section, "iTabSelected"sv, styles.colors.iTabSelected);
        SetTOMLValue(section, "iTabSelectedOverline"sv, styles.colors.iTabSelectedOverline);
        SetTOMLValue(section, "iTabDimmed"sv, styles.colors.iTabDimmed);
        SetTOMLValue(section, "iTabDimmedSelected"sv, styles.colors.iTabDimmedSelected);
        SetTOMLValue(section, "iTabDimmedSelectedOverline"sv, styles.colors.iTabDimmedSelectedOverline);
        SetTOMLValue(section, "iTabUnfocused"sv, styles.colors.iTabUnfocused);
        SetTOMLValue(section, "iTabUnfocusedActive"sv, styles.colors.iTabUnfocusedActive);
        SetTOMLValue(section, "iPlotLines"sv, styles.colors.iPlotLines);
        SetTOMLValue(section, "iPlotLinesHovered"sv, styles.colors.iPlotLinesHovered);
        SetTOMLValue(section, "iPlotHistogram"sv, styles.colors.iPlotHistogram);
        SetTOMLValue(section, "iPlotHistogramHovered"sv, styles.colors.iPlotHistogramHovered);
        SetTOMLValue(section, "iTableHeaderBg"sv, styles.colors.iTableHeaderBg);
        SetTOMLValue(section, "iTableBorderStrong"sv, styles.colors.iTableBorderStrong);
        SetTOMLValue(section, "iTableBorderLight"sv, styles.colors.iTableBorderLight);
        SetTOMLValue(section, "iTableRowBg"sv, styles.colors.iTableRowBg);
        SetTOMLValue(section, "iTableRowBgAlt"sv, styles.colors.iTableRowBgAlt);
        SetTOMLValue(section, "iTextSelectedBg"sv, styles.colors.iTextSelectedBg);
        SetTOMLValue(section, "iDragDropTarget"sv, styles.colors.iDragDropTarget);
        SetTOMLValue(section, "iNavHighlight"sv, styles.colors.iNavHighlight);
        SetTOMLValue(section, "iNavWindowingHighlight"sv, styles.colors.iNavWindowingHighlight);
        SetTOMLValue(section, "iNavWindowingDimBg"sv, styles.colors.iNavWindowingDimBg);
        SetTOMLValue(section, "iModalWindowDimBg"sv, styles.colors.iModalWindowDimBg);
        SetTOMLSection(data, "Colors"sv, std::move(section));
    }
    SaveTOMLFile(a_path, data);
}
