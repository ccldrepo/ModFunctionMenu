#pragma once

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
    std::string Lookup(std::string_view a_key) const
    {
        auto it = _map.find(a_key);
        if (it != _map.end()) {
            return it->second;
        }
        return std::string{ a_key };
    }

    // Visit translation map.
    template <class Visitor>
    void Visit(Visitor&& a_visitor) const
    {
        for (auto&& [key, value] : _map) {
            a_visitor(key, value);
        }
    }

private:
    Translation() = default;

    void Load(bool a_abort);
    void LoadImpl(const std::filesystem::path& a_path);

    static inline std::unique_ptr<Translation> _singleton;

    absl::flat_hash_map<std::string, std::string> _map;
};

inline std::string operator""_T(const char* a_str, std::size_t a_len)
{
    return Translation::GetSingleton()->Lookup(std::string_view{ a_str, a_len });
}
