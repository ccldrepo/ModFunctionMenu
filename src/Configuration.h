#pragma once

class Configuration
{
public:
    [[nodiscard]] static const Configuration* GetSingleton() { return _singleton.get(); }

    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

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
        };

        struct Gamepad
        {
            std::uint32_t iHotkey{ 0 };
            std::uint32_t iModifier{ 0 };
        };

        Keyboard keyboard;
        Gamepad  gamepad;
    };

    General  general;
    Controls controls;

private:
    Configuration() = default;

    using LoadImplFunc = void (Configuration::*)(const std::filesystem::path&);
    using SaveImplFunc = void (Configuration::*)(const std::filesystem::path&) const;

    void Load(const std::filesystem::path& a_path, LoadImplFunc a_func, bool a_abort);
    void Save(const std::filesystem::path& a_path, SaveImplFunc a_func, bool a_abort) const;

    void LoadImpl(const std::filesystem::path& a_path);
    void SaveImpl(const std::filesystem::path& a_path) const;

    static inline std::unique_ptr<Configuration> _singleton;

    static inline const std::filesystem::path _path{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu.toml"sv };
};
