#pragma once

#include <PCH.h>

#include <absl/container/flat_hash_map.h>

class Translation
{
public:
    [[nodiscard]] static const Translation* GetSingleton() { return _singleton.get(); }

    Translation(const Translation&) = delete;
    Translation(Translation&&) = delete;
    Translation& operator=(const Translation&) = delete;
    Translation& operator=(Translation&&) = delete;

    // Initialize or reload translation.
    static void Init(bool a_abort = true);

    // Lookup translation text.
    const std::string& Lookup(const std::string& a_key) const;

private:
    Translation() = default;

    void Load(bool a_abort);

    void LoadImpl(const std::filesystem::path& path);

    static inline std::unique_ptr<Translation> _singleton;

    absl::flat_hash_map<std::string, std::string> _map;
};
