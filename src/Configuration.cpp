#include "Configuration.h"

#include <toml++/toml.hpp>

#include "Util/TOML.h"

void Configuration::Init(bool a_abort)
{
    auto tmp = std::unique_ptr<Configuration>{ new Configuration };
    if (std::filesystem::exists(_path)) {
        tmp->Load(_path, &Configuration::LoadImpl, a_abort);
    } else {
        // Export default config if config file not exists.
        tmp->Save(_path, &Configuration::SaveImpl, a_abort);
    }
    if (std::filesystem::exists(_path_styles)) {
        tmp->Load(_path_styles, &Configuration::LoadImpl_Styles, a_abort);
    } else {
        // Export default config if config file not exists.
        //tmp->Save(_path_styles, &Configuration::SaveImpl, a_abort);
    }
    _singleton = std::move(tmp);
}

void Configuration::Load(const std::filesystem::path& a_path, LoadImplFunc a_func, bool a_abort)
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

void Configuration::Save(const std::filesystem::path& a_path, SaveImplFunc a_func, bool a_abort) const
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
        GetTOMLValue(*section, "sFont"sv, general.sFont);
    }

    if (auto section = GetTOMLSection(data, "Controls"sv)) {
        if (auto subsection = GetTOMLSection(*section, "Keyboard"sv)) {
            GetTOMLValue(*subsection, "iHotkey"sv, controls.keyboard.iHotkey);
            GetTOMLValue(*subsection, "iModifier"sv, controls.keyboard.iModifier);
        }

        if (auto subsection = GetTOMLSection(*section, "Gamepad"sv)) {
            GetTOMLValue(*subsection, "iHotkey"sv, controls.gamepad.iHotkey);
            GetTOMLValue(*subsection, "iModifier"sv, controls.gamepad.iModifier);
        }
    }
}

void Configuration::SaveImpl(const std::filesystem::path& a_path) const
{
    toml::table data;
    {
        toml::table section;
        SetTOMLValue(section, "sLanguage"sv, general.sLanguage);
        SetTOMLValue(section, "sFont"sv, general.sFont);
        SetTOMLSection(data, "General"sv, std::move(section));
    }
    {
        toml::table section;
        {
            toml::table subsection;
            SetTOMLValue(subsection, "iHotkey"sv, controls.keyboard.iHotkey);
            SetTOMLValue(subsection, "iModifier"sv, controls.keyboard.iModifier);
            SetTOMLSection(section, "Keyboard"sv, std::move(subsection));
        }
        {
            toml::table subsection;
            SetTOMLValue(subsection, "iHotkey"sv, controls.gamepad.iHotkey);
            SetTOMLValue(subsection, "iModifier"sv, controls.gamepad.iModifier);
            SetTOMLSection(section, "Gamepad"sv, std::move(subsection));
        }
        SetTOMLSection(data, "Controls"sv, std::move(section));
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
        GetTOMLValue(*section, "iTab"sv, styles.colors.iTab);
        GetTOMLValue(*section, "iTabHovered"sv, styles.colors.iTabHovered);
        GetTOMLValue(*section, "iTabActive"sv, styles.colors.iTabActive);
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
