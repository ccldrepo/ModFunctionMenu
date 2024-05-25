#include "Translation.h"

#include <absl/strings/ascii.h>

namespace
{
    inline std::string GetGameLanguage()
    {
        auto setting = RE::GetINISetting("sLanguage:General");
        if (setting && setting->GetType() == RE::Setting::Type::kString) {
            return absl::AsciiStrToUpper(setting->GetString());
        }
        return "ENGLISH";
    }

    inline std::filesystem::path GetTranslationPath(std::string_view a_name, std::string_view a_language)
    {
        auto path = StrToPath(std::format("Data/Interface/Translations/{}_{}.txt", a_name, a_language));
        if (std::filesystem::exists(path)) {
            return path;
        }
        return StrToPath(std::format("Data/Interface/Translations/{}_ENGLISH.txt", a_name));
    }

    inline std::string NormalizeLine(std::wstring_view a_line)
    {
        while (a_line.ends_with(L'\r') || a_line.ends_with(L'\n')) {
            a_line.remove_suffix(1);
        }

        auto text = SKSE::stl::utf16_to_utf8(a_line);
        if (!text) {
            return {};
        }
        return *std::move(text);
    }
}

void Translation::Init(bool a_abort)
{
    auto tmp = std::unique_ptr<Translation>{ new Translation };
    tmp->Load(a_abort);
    _singleton = std::move(tmp);
}

const std::string& Translation::Lookup(const std::string& a_key) const
{
    auto it = _map.find(a_key);
    if (it == _map.end()) {
        return a_key;
    }
    return it->second;
}

void Translation::Load(bool a_abort)
{
    auto path = GetTranslationPath("ccld_ModFunctionMenu"sv, GetGameLanguage());
    try {
        LoadImpl(path);
        SKSE::log::info("Successfully loaded translation from \"{}\".", PathToStr(path));
    } catch (const std::exception& e) {
        auto msg = std::format("Failed to load translation from \"{}\": {}.", PathToStr(path), e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    }
}

void Translation::LoadImpl(const std::filesystem::path& path)
{
    std::wifstream file{ path, std::ios_base::in | std::ios_base::binary };
    if (!file) {
        throw std::runtime_error("File could not be opened for reading");
    }

    if (file.get() != 0xFEFF) {
        throw std::runtime_error("BOM check failed. File must be encoded in UTF-16 LE");
    }

    for (std::wstring line; std::getline(file, line);) {
        auto text = NormalizeLine(line);
        if (text.empty()) {
            continue;
        }

        if (text[0] != '$') {
            auto msg = std::format("Translation key must start with leading '$': {}", text);
            throw std::runtime_error(msg);
        }

        auto pos = text.find_first_of('\t');
        if (pos == std::string::npos) {
            auto msg = std::format("Translation key and value must be seperated with 'TAB': {}", text);
            throw std::runtime_error(msg);
        }

        auto key = text.substr(0, pos);
        auto value = std::move(text).substr(pos + 1);
        _map.insert_or_assign(std::move(key), std::move(value));
    }
}
