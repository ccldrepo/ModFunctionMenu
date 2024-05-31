#include "Translation.h"

#include <absl/strings/ascii.h>
#include <absl/strings/str_split.h>

#include "Configuration.h"

namespace
{
    inline std::string GetGameLanguage()
    {
        auto setting = RE::GetINISetting("sLanguage:General");
        if (setting && setting->GetType() == RE::Setting::Type::kString) {
            return absl::AsciiStrToLower(setting->GetString());
        }
        return "english";
    }

    inline std::string GetUserLanguage()
    {
        auto config = Configuration::GetSingleton();
        if (!config->general.sLanguage.empty()) {
            return absl::AsciiStrToLower(config->general.sLanguage);
        }
        return GetGameLanguage();
    }

    inline std::filesystem::path GetTranslationPath(std::string_view a_name, std::string_view a_language)
    {
        auto path = StrToPath(std::format("Data/Interface/Translations/{}_{}.txt", a_name, a_language));
        if (std::filesystem::exists(path)) {
            return path;
        }
        return StrToPath(std::format("Data/Interface/Translations/{}_english.txt", a_name));
    }

    inline std::string ReadUTF16LEFile(const std::filesystem::path& a_path)
    {
        const auto size = std::filesystem::file_size(a_path);

        if (size == 0) {
            return {};
        }

        if (size % 2 != 0) {
            throw std::runtime_error("Size check failed. File must be encoded in UTF-16 LE");
        }

        const auto wsize = static_cast<std::size_t>(size / 2);
        const auto wdata = std::make_unique_for_overwrite<wchar_t[]>(wsize);

        if (std::ifstream file{ a_path, std::ios_base::in | std::ios_base::binary }) {
            file.read(reinterpret_cast<char*>(wdata.get()), static_cast<std::streamsize>(size));
        } else {
            throw std::runtime_error("File could not be opened for reading");
        }

        if (wdata[0] != static_cast<wchar_t>(0xFEFF)) {
            throw std::runtime_error("BOM check failed. File must be encoded in UTF-16 LE");
        }

        std::wstring_view wstr{ wdata.get() + 1, wsize - 1 };
        if (auto str = SKSE::stl::utf16_to_utf8(wstr)) {
            return *std::move(str);
        }
        return {};
    }
}

void Translation::Init(bool a_abort)
{
    auto tmp = std::unique_ptr<Translation>{ new Translation };
    tmp->Load(a_abort);
    _singleton = std::move(tmp);
}

std::string Translation::Lookup(std::string_view a_key) const
{
    auto it = _map.find(a_key);
    if (it == _map.end()) {
        return std::string{ a_key };
    }
    return it->second;
}

void Translation::Load(bool a_abort)
{
    auto path = GetTranslationPath("ccld_ModFunctionMenu"sv, GetUserLanguage());
    try {
        LoadImpl(path);
        SKSE::log::info("Successfully loaded translation from \"{}\".", PathToStr(path));
    } catch (const std::exception& e) {
        auto msg = std::format("Failed to load translation from \"{}\": {}.", PathToStr(path), e.what());
        SKSE::stl::report_fatal_error(msg, a_abort);
    }
}

void Translation::LoadImpl(const std::filesystem::path& a_path)
{
    auto text = ReadUTF16LEFile(a_path);
    for (std::string_view line : absl::StrSplit(text, "\r\n"sv, absl::SkipWhitespace())) {
        if (line[0] != '$') {
            auto msg = std::format("Translation key must start with leading '$': {}", line);
            throw std::runtime_error(msg);
        }

        auto pos = line.find_first_of('\t');
        if (pos == std::string::npos) {
            auto msg = std::format("Translation key and value must be seperated with 'TAB': {}", line);
            throw std::runtime_error(msg);
        }

        auto key = line.substr(0, pos);
        auto value = std::move(line).substr(pos + 1);
        _map.insert_or_assign(std::move(key), std::move(value));
    }
}
