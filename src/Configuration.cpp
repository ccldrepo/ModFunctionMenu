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
