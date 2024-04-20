#pragma once

#include <PCH.h>

class Configuration
{
public:
    static const Configuration* GetSingleton() { return config.get(); }

    // Initialize or reload configuration.
    static void Init(bool a_throw = false);

    struct Default
    {
        static constexpr std::uint32_t iHotkey{ REX::W32::DIK_F1 };
        static constexpr std::uint32_t iModifier{ 0 };
    };

    std::uint32_t iHotkey{ Default::iHotkey };
    std::uint32_t iModifier{ Default::iModifier };

private:
    Configuration() = default;

    Configuration(const Configuration&) = delete;
    Configuration(Configuration&&) = delete;
    Configuration& operator=(const Configuration&) = delete;
    Configuration& operator=(Configuration&&) = delete;

    void Load(bool a_throw);
    void Save(bool a_throw) const;

    void LoadImpl();
    void SaveImpl() const;

    static inline std::unique_ptr<Configuration> config{ nullptr };

    static inline const std::filesystem::path path{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu.toml"sv };
};
