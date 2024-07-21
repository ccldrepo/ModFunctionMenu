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
        // Export default config if config file does not exist.
        tmp->Save(&Configuration::SaveImpl, _path, a_abort);
    }

    if (std::filesystem::exists(_path_fonts)) {
        tmp->Load(&Configuration::LoadImpl_Fonts, _path_fonts, a_abort);
    } else {
        // Export default config if config file does not exist.
        tmp->Save(&Configuration::SaveImpl_Fonts, _path_fonts, a_abort);
    }

    if (std::filesystem::exists(_path_styles)) {
        tmp->Load(&Configuration::LoadImpl_Styles, _path_styles, a_abort);
    } else {
        // Export default config if config file does not exist.
        tmp->Save(&Configuration::SaveImpl_Styles, _path_styles, a_abort);
    }

    _singleton = std::move(tmp);
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
    auto data = TOML::LoadFile(a_path);

    if (auto section = TOML::GetSection(data, "General"sv)) {
        TOML::GetValue(section, "sLanguage"sv, general.sLanguage);
    }

    if (auto section = TOML::GetSection(data, "Controls"sv)) {
        if (auto subsection = TOML::GetSection(*section, "Keyboard"sv)) {
            TOML::GetValue(subsection, "iHotkey"sv, controls.keyboard.iHotkey);
            TOML::GetValue(subsection, "iModifier"sv, controls.keyboard.iModifier);
            TOML::GetValue(subsection, "iExtraExit"sv, controls.keyboard.iExtraExit);
        }

        if (auto subsection = TOML::GetSection(*section, "Gamepad"sv)) {
            TOML::GetValue(subsection, "iHotkey"sv, controls.gamepad.iHotkey);
            TOML::GetValue(subsection, "iModifier"sv, controls.gamepad.iModifier);
            TOML::GetValue(subsection, "iExtraExit"sv, controls.gamepad.iExtraExit);
        }
    }
}

void Configuration::SaveImpl(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        TOML::SetValue(section, "sLanguage"sv, general.sLanguage);
        TOML::SetSection(data, "General"sv, std::move(section));
    }
    {
        toml::table section;
        {
            toml::table subsection;
            TOML::SetValue(subsection, "iHotkey"sv, controls.keyboard.iHotkey);
            TOML::SetValue(subsection, "iModifier"sv, controls.keyboard.iModifier);
            TOML::SetValue(subsection, "iExtraExit"sv, controls.keyboard.iExtraExit);
            TOML::SetSection(section, "Keyboard"sv, std::move(subsection));
        }
        {
            toml::table subsection;
            TOML::SetValue(subsection, "iHotkey"sv, controls.gamepad.iHotkey);
            TOML::SetValue(subsection, "iModifier"sv, controls.gamepad.iModifier);
            TOML::SetValue(subsection, "iExtraExit"sv, controls.gamepad.iExtraExit);
            TOML::SetSection(section, "Gamepad"sv, std::move(subsection));
        }
        TOML::SetSection(data, "Controls"sv, std::move(section));
    }
    TOML::SaveFile(a_path, data);
}

void Configuration::LoadImpl_Fonts(const std::filesystem::path& a_path)
{
    auto data = TOML::LoadFile(a_path);

    if (auto section = TOML::GetSection(data, "General"sv)) {
        TOML::GetValue(section, "sFont"sv, fonts.general.sFont);
        CheckRegularFile(StrToPath(fonts.general.sFont));
        TOML::GetValue(section, "fSize"sv, fonts.general.fSize);
    }
}

void Configuration::SaveImpl_Fonts(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        TOML::SetValue(section, "sFont"sv, fonts.general.sFont);
        TOML::SetValue(section, "fSize"sv, fonts.general.fSize);
        TOML::SetSection(data, "General"sv, std::move(section));
    }
    TOML::SaveFile(a_path, data);
}

void Configuration::LoadImpl_Styles(const std::filesystem::path& a_path)
{
    auto data = TOML::LoadFile(a_path);

    if (auto section = TOML::GetSection(data, "Colors"sv)) {
        TOML::GetValue(section, "iText"sv, styles.colors.iText);
        TOML::GetValue(section, "iTextDisabled"sv, styles.colors.iTextDisabled);
        TOML::GetValue(section, "iWindowBg"sv, styles.colors.iWindowBg);
        TOML::GetValue(section, "iChildBg"sv, styles.colors.iChildBg);
        TOML::GetValue(section, "iPopupBg"sv, styles.colors.iPopupBg);
        TOML::GetValue(section, "iBorder"sv, styles.colors.iBorder);
        TOML::GetValue(section, "iBorderShadow"sv, styles.colors.iBorderShadow);
        TOML::GetValue(section, "iFrameBg"sv, styles.colors.iFrameBg);
        TOML::GetValue(section, "iFrameBgHovered"sv, styles.colors.iFrameBgHovered);
        TOML::GetValue(section, "iFrameBgActive"sv, styles.colors.iFrameBgActive);
        TOML::GetValue(section, "iTitleBg"sv, styles.colors.iTitleBg);
        TOML::GetValue(section, "iTitleBgActive"sv, styles.colors.iTitleBgActive);
        TOML::GetValue(section, "iTitleBgCollapsed"sv, styles.colors.iTitleBgCollapsed);
        TOML::GetValue(section, "iMenuBarBg"sv, styles.colors.iMenuBarBg);
        TOML::GetValue(section, "iScrollbarBg"sv, styles.colors.iScrollbarBg);
        TOML::GetValue(section, "iScrollbarGrab"sv, styles.colors.iScrollbarGrab);
        TOML::GetValue(section, "iScrollbarGrabHovered"sv, styles.colors.iScrollbarGrabHovered);
        TOML::GetValue(section, "iScrollbarGrabActive"sv, styles.colors.iScrollbarGrabActive);
        TOML::GetValue(section, "iCheckMark"sv, styles.colors.iCheckMark);
        TOML::GetValue(section, "iSliderGrab"sv, styles.colors.iSliderGrab);
        TOML::GetValue(section, "iSliderGrabActive"sv, styles.colors.iSliderGrabActive);
        TOML::GetValue(section, "iButton"sv, styles.colors.iButton);
        TOML::GetValue(section, "iButtonHovered"sv, styles.colors.iButtonHovered);
        TOML::GetValue(section, "iButtonActive"sv, styles.colors.iButtonActive);
        TOML::GetValue(section, "iHeader"sv, styles.colors.iHeader);
        TOML::GetValue(section, "iHeaderHovered"sv, styles.colors.iHeaderHovered);
        TOML::GetValue(section, "iHeaderActive"sv, styles.colors.iHeaderActive);
        TOML::GetValue(section, "iSeparator"sv, styles.colors.iSeparator);
        TOML::GetValue(section, "iSeparatorHovered"sv, styles.colors.iSeparatorHovered);
        TOML::GetValue(section, "iSeparatorActive"sv, styles.colors.iSeparatorActive);
        TOML::GetValue(section, "iResizeGrip"sv, styles.colors.iResizeGrip);
        TOML::GetValue(section, "iResizeGripHovered"sv, styles.colors.iResizeGripHovered);
        TOML::GetValue(section, "iResizeGripActive"sv, styles.colors.iResizeGripActive);
        TOML::GetValue(section, "iTabHovered"sv, styles.colors.iTabHovered);
        TOML::GetValue(section, "iTab"sv, styles.colors.iTab);
        TOML::GetValue(section, "iTabSelected"sv, styles.colors.iTabSelected);
        TOML::GetValue(section, "iTabSelectedOverline"sv, styles.colors.iTabSelectedOverline);
        TOML::GetValue(section, "iTabDimmed"sv, styles.colors.iTabDimmed);
        TOML::GetValue(section, "iTabDimmedSelected"sv, styles.colors.iTabDimmedSelected);
        TOML::GetValue(section, "iTabDimmedSelectedOverline"sv, styles.colors.iTabDimmedSelectedOverline);
        TOML::GetValue(section, "iTabUnfocused"sv, styles.colors.iTabUnfocused);
        TOML::GetValue(section, "iTabUnfocusedActive"sv, styles.colors.iTabUnfocusedActive);
        TOML::GetValue(section, "iPlotLines"sv, styles.colors.iPlotLines);
        TOML::GetValue(section, "iPlotLinesHovered"sv, styles.colors.iPlotLinesHovered);
        TOML::GetValue(section, "iPlotHistogram"sv, styles.colors.iPlotHistogram);
        TOML::GetValue(section, "iPlotHistogramHovered"sv, styles.colors.iPlotHistogramHovered);
        TOML::GetValue(section, "iTableHeaderBg"sv, styles.colors.iTableHeaderBg);
        TOML::GetValue(section, "iTableBorderStrong"sv, styles.colors.iTableBorderStrong);
        TOML::GetValue(section, "iTableBorderLight"sv, styles.colors.iTableBorderLight);
        TOML::GetValue(section, "iTableRowBg"sv, styles.colors.iTableRowBg);
        TOML::GetValue(section, "iTableRowBgAlt"sv, styles.colors.iTableRowBgAlt);
        TOML::GetValue(section, "iTextSelectedBg"sv, styles.colors.iTextSelectedBg);
        TOML::GetValue(section, "iDragDropTarget"sv, styles.colors.iDragDropTarget);
        TOML::GetValue(section, "iNavHighlight"sv, styles.colors.iNavHighlight);
        TOML::GetValue(section, "iNavWindowingHighlight"sv, styles.colors.iNavWindowingHighlight);
        TOML::GetValue(section, "iNavWindowingDimBg"sv, styles.colors.iNavWindowingDimBg);
        TOML::GetValue(section, "iModalWindowDimBg"sv, styles.colors.iModalWindowDimBg);
    }
}

void Configuration::SaveImpl_Styles(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        TOML::SetValue(section, "iText"sv, styles.colors.iText);
        TOML::SetValue(section, "iTextDisabled"sv, styles.colors.iTextDisabled);
        TOML::SetValue(section, "iWindowBg"sv, styles.colors.iWindowBg);
        TOML::SetValue(section, "iChildBg"sv, styles.colors.iChildBg);
        TOML::SetValue(section, "iPopupBg"sv, styles.colors.iPopupBg);
        TOML::SetValue(section, "iBorder"sv, styles.colors.iBorder);
        TOML::SetValue(section, "iBorderShadow"sv, styles.colors.iBorderShadow);
        TOML::SetValue(section, "iFrameBg"sv, styles.colors.iFrameBg);
        TOML::SetValue(section, "iFrameBgHovered"sv, styles.colors.iFrameBgHovered);
        TOML::SetValue(section, "iFrameBgActive"sv, styles.colors.iFrameBgActive);
        TOML::SetValue(section, "iTitleBg"sv, styles.colors.iTitleBg);
        TOML::SetValue(section, "iTitleBgActive"sv, styles.colors.iTitleBgActive);
        TOML::SetValue(section, "iTitleBgCollapsed"sv, styles.colors.iTitleBgCollapsed);
        TOML::SetValue(section, "iMenuBarBg"sv, styles.colors.iMenuBarBg);
        TOML::SetValue(section, "iScrollbarBg"sv, styles.colors.iScrollbarBg);
        TOML::SetValue(section, "iScrollbarGrab"sv, styles.colors.iScrollbarGrab);
        TOML::SetValue(section, "iScrollbarGrabHovered"sv, styles.colors.iScrollbarGrabHovered);
        TOML::SetValue(section, "iScrollbarGrabActive"sv, styles.colors.iScrollbarGrabActive);
        TOML::SetValue(section, "iCheckMark"sv, styles.colors.iCheckMark);
        TOML::SetValue(section, "iSliderGrab"sv, styles.colors.iSliderGrab);
        TOML::SetValue(section, "iSliderGrabActive"sv, styles.colors.iSliderGrabActive);
        TOML::SetValue(section, "iButton"sv, styles.colors.iButton);
        TOML::SetValue(section, "iButtonHovered"sv, styles.colors.iButtonHovered);
        TOML::SetValue(section, "iButtonActive"sv, styles.colors.iButtonActive);
        TOML::SetValue(section, "iHeader"sv, styles.colors.iHeader);
        TOML::SetValue(section, "iHeaderHovered"sv, styles.colors.iHeaderHovered);
        TOML::SetValue(section, "iHeaderActive"sv, styles.colors.iHeaderActive);
        TOML::SetValue(section, "iSeparator"sv, styles.colors.iSeparator);
        TOML::SetValue(section, "iSeparatorHovered"sv, styles.colors.iSeparatorHovered);
        TOML::SetValue(section, "iSeparatorActive"sv, styles.colors.iSeparatorActive);
        TOML::SetValue(section, "iResizeGrip"sv, styles.colors.iResizeGrip);
        TOML::SetValue(section, "iResizeGripHovered"sv, styles.colors.iResizeGripHovered);
        TOML::SetValue(section, "iResizeGripActive"sv, styles.colors.iResizeGripActive);
        TOML::SetValue(section, "iTabHovered"sv, styles.colors.iTabHovered);
        TOML::SetValue(section, "iTab"sv, styles.colors.iTab);
        TOML::SetValue(section, "iTabSelected"sv, styles.colors.iTabSelected);
        TOML::SetValue(section, "iTabSelectedOverline"sv, styles.colors.iTabSelectedOverline);
        TOML::SetValue(section, "iTabDimmed"sv, styles.colors.iTabDimmed);
        TOML::SetValue(section, "iTabDimmedSelected"sv, styles.colors.iTabDimmedSelected);
        TOML::SetValue(section, "iTabDimmedSelectedOverline"sv, styles.colors.iTabDimmedSelectedOverline);
        TOML::SetValue(section, "iTabUnfocused"sv, styles.colors.iTabUnfocused);
        TOML::SetValue(section, "iTabUnfocusedActive"sv, styles.colors.iTabUnfocusedActive);
        TOML::SetValue(section, "iPlotLines"sv, styles.colors.iPlotLines);
        TOML::SetValue(section, "iPlotLinesHovered"sv, styles.colors.iPlotLinesHovered);
        TOML::SetValue(section, "iPlotHistogram"sv, styles.colors.iPlotHistogram);
        TOML::SetValue(section, "iPlotHistogramHovered"sv, styles.colors.iPlotHistogramHovered);
        TOML::SetValue(section, "iTableHeaderBg"sv, styles.colors.iTableHeaderBg);
        TOML::SetValue(section, "iTableBorderStrong"sv, styles.colors.iTableBorderStrong);
        TOML::SetValue(section, "iTableBorderLight"sv, styles.colors.iTableBorderLight);
        TOML::SetValue(section, "iTableRowBg"sv, styles.colors.iTableRowBg);
        TOML::SetValue(section, "iTableRowBgAlt"sv, styles.colors.iTableRowBgAlt);
        TOML::SetValue(section, "iTextSelectedBg"sv, styles.colors.iTextSelectedBg);
        TOML::SetValue(section, "iDragDropTarget"sv, styles.colors.iDragDropTarget);
        TOML::SetValue(section, "iNavHighlight"sv, styles.colors.iNavHighlight);
        TOML::SetValue(section, "iNavWindowingHighlight"sv, styles.colors.iNavWindowingHighlight);
        TOML::SetValue(section, "iNavWindowingDimBg"sv, styles.colors.iNavWindowingDimBg);
        TOML::SetValue(section, "iModalWindowDimBg"sv, styles.colors.iModalWindowDimBg);
        TOML::SetSection(data, "Colors"sv, std::move(section));
    }
    TOML::SaveFile(a_path, data);
}
