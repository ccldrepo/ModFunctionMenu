#include "Configuration.h"

#include <toml++/toml.hpp>

#include "Util/TOML.h"

void Configuration::Init(bool a_abort)
{
    auto tmp = std::unique_ptr<Configuration>{ new Configuration };
    if (std::filesystem::exists(_path)) {
        tmp->Load(a_abort);
    } else {
        // Export default config if config file not exists.
        tmp->Save(a_abort);
    }
    _singleton = std::move(tmp);
}

void Configuration::Load(bool a_abort)
{
    try {
        LoadImpl();
        SKSE::log::info("Successfully loaded configuration from \"{}\".", PathToStr(_path));
    } catch (const toml::parse_error& e) {
        auto msg = std::format("Failed to load configuration from \"{}\" (error occurred at line {}, column {}): {}.",
            PathToStr(_path), e.source().begin.line, e.source().begin.column, e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    } catch (const std::system_error& e) {
        auto msg = std::format("Failed to load configuration from \"{}\": {}.", PathToStr(_path),
            SKSE::stl::ansi_to_utf8(e.what()).value_or(e.what()));
        SKSE::stl::report_fatal_error(msg, a_abort);
    } catch (const std::exception& e) {
        auto msg = std::format("Failed to load configuration from \"{}\": {}.", PathToStr(_path), e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    }
}

void Configuration::Save(bool a_abort) const
{
    try {
        SaveImpl();
        SKSE::log::info("Successfully saved configuration to \"{}\".", PathToStr(_path));
    } catch (const std::system_error& e) {
        auto msg = std::format("Failed to save configuration to \"{}\": {}.", PathToStr(_path),
            SKSE::stl::ansi_to_utf8(e.what()).value_or(e.what()));
        SKSE::stl::report_fatal_error(msg, a_abort);
    } catch (const std::exception& e) {
        auto msg = std::format("Failed to save configuration to \"{}\": {}.", PathToStr(_path), e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    }
}

void Configuration::LoadImpl()
{
    auto data = LoadTOMLFile(_path);

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

void Configuration::SaveImpl() const
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
    SaveTOMLFile(_path, data);
}
