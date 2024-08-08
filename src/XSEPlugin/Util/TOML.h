#pragma once

#include <array>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <toml++/toml.hpp>

namespace TOML
{
    template <class T>
    concept Scalar = std::is_arithmetic_v<T> || std::is_same_v<T, std::string>;

    class Error : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    template <class T>
    struct Validator
    {
        [[nodiscard]] std::pair<bool, std::string> operator()([[maybe_unused]] const T& a_value) const
        {
            return { true, std::string() };
        }
    };

    struct DirectoryValidator
    {
        [[nodiscard]] std::pair<bool, std::string> operator()(const std::string& a_value) const
        {
            auto path = StrToPath(a_value);
            auto st = std::filesystem::status(path);

            if (!std::filesystem::exists(st)) {
                return { false, std::format("'{}' does not exist", a_value) };
            }

            if (!std::filesystem::is_directory(st)) {
                return { false, std::format("'{}' is not a directory", a_value) };
            }

            return { true, std::string() };
        }
    };

    struct RegularFileValidator
    {
        [[nodiscard]] std::pair<bool, std::string> operator()(const std::string& a_value) const
        {
            auto path = StrToPath(a_value);
            auto st = std::filesystem::status(path);

            if (!std::filesystem::exists(st)) {
                return { false, std::format("'{}' does not exist", a_value) };
            }

            if (!std::filesystem::is_regular_file(st)) {
                return { false, std::format("'{}' is not a regular file", a_value) };
            }

            return { true, std::string() };
        }
    };

    struct LogLevelValidator
    {
        [[nodiscard]] std::pair<bool, std::string> operator()(const std::string& a_value) const
        {
            if (a_value.empty()) {
                return { true, std::string() };
            }

            constexpr std::array log_levels{ "trace"sv, "debug"sv, "info"sv, "warn"sv, "error"sv, "critical"sv };
            for (std::string_view log_level : log_levels) {
                if (a_value == log_level) {
                    return { true, std::string() };
                }
            }
            return { false, std::format("'{}' is not a valid log level", a_value) };
        }
    };

    [[nodiscard]] inline toml::table LoadFile(const std::filesystem::path& a_path)
    {
        const auto size = static_cast<std::size_t>(std::filesystem::file_size(a_path));
        const auto data = std::make_unique_for_overwrite<char[]>(size);

        if (std::ifstream file{ a_path, std::ios_base::in | std::ios_base::binary }) {
            file.read(data.get(), static_cast<std::streamsize>(size));
        } else {
            throw Error("File could not be opened for reading");
        }

        std::string_view doc{ data.get(), size };
        return toml::parse(doc, a_path.native());
    }

    [[nodiscard]] inline toml::table LoadFile(const std::string& a_path) = delete;
    [[nodiscard]] inline toml::table LoadFile(std::string_view a_path) = delete;
    [[nodiscard]] inline toml::table LoadFile(const char* a_path) = delete;

    inline void SaveFile(const std::filesystem::path& a_path, const toml::table& a_table)
    {
        if (std::ofstream file{ a_path }) {
            file << a_table << std::endl;
        } else {
            throw Error("File could not be opened for writing");
        }
    }

    inline void SaveFile(const std::string& a_path, const toml::table& a_table) = delete;
    inline void SaveFile(std::string_view a_path, const toml::table& a_table) = delete;
    inline void SaveFile(const char* a_path, const toml::table& a_table) = delete;

    template <bool required = false>
    [[nodiscard]] inline const toml::table* GetSection(const toml::table& a_table, std::string_view a_key)
    {
        auto node = a_table.get(a_key);
        if (!node) {
            if constexpr (required) {
                throw Error(std::format("'{}' is required", a_key));
            } else {
                return nullptr;
            }
        }

        auto section = node->as_table();
        if (!section) {
            auto msg = std::format("'{}' is not a valid section", a_key);
            throw toml::parse_error(msg.c_str(), node->source());
        }
        return section;
    }

    [[nodiscard]] inline const toml::table* GetSectionRequired(const toml::table& a_table, std::string_view a_key)
    {
        return GetSection<true>(a_table, a_key);
    }

    inline void SetSection(toml::table& a_table, std::string_view a_key, toml::table&& a_section)
    {
        auto [pos, ok] = a_table.emplace(a_key, std::move(a_section));
        if (!ok) {
            throw Error(std::format("'{}' exists", a_key));
        }
    }

    template <Scalar T, class V = Validator<T>, bool required = false>
    inline void GetValue(const toml::table& a_table, std::string_view a_key, T& a_target, V&& a_validator = V())
    {
        auto node = a_table.get(a_key);
        if (!node) {
            if constexpr (required) {
                throw Error(std::format("'{}' is required", a_key));
            } else {
                return;  // Leave target unchanged.
            }
        }

        auto value = node->value<T>();
        if (!value) {
            if constexpr (std::is_same_v<T, bool>) {
                if (!node->is_boolean()) {
                    auto msg = std::format("'{}' is not a valid bool", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            } else if constexpr (std::is_integral_v<T>) {
                if (!node->is_integer()) {
                    auto msg = std::format("'{}' is not a valid integer", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            } else if constexpr (std::is_floating_point_v<T>) {
                if (!node->is_floating_point()) {
                    auto msg = std::format("'{}' is not a valid float", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (!node->is_string()) {
                    auto msg = std::format("'{}' is not a valid string", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            }

            auto msg = std::format("Invalid '{}'", a_key);
            throw toml::parse_error(msg.c_str(), node->source());
        }

        if (auto [ok, msg] = a_validator(*value); !ok) {
            throw toml::parse_error(msg.c_str(), node->source());
        }
        a_target = *std::move(value);
    }

    template <Scalar T, class V = Validator<T>, bool required = false>
    inline void GetValue(const toml::table* a_table, std::string_view a_key, T& a_target, V&& a_validator = V())
    {
        return GetValue<T, V, required>(*a_table, a_key, a_target, std::forward<V>(a_validator));
    }

    template <Scalar T, class V = Validator<T>, bool required = false>
    inline void GetValue(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target,
        V&& a_validator = V())
    {
        auto node = a_table.get(a_key);
        if (!node) {
            if constexpr (required) {
                throw Error(std::format("'{}' is required", a_key));
            } else {
                return;  // Leave target unchanged.
            }
        }

        auto arr = node->as_array();
        if (!arr) {
            auto msg = std::format("'{}' is not a valid array", a_key);
            throw toml::parse_error(msg.c_str(), node->source());
        }

        a_target.clear();
        a_target.reserve(arr->size());
        for (const auto& ele : *arr) {
            auto value = ele.value<T>();
            if (!value) {
                if constexpr (std::is_same_v<T, bool>) {
                    if (!node->is_boolean()) {
                        auto msg = std::format("'{}' is not a valid array of bool", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                } else if constexpr (std::is_integral_v<T>) {
                    if (!node->is_integer()) {
                        auto msg = std::format("'{}' is not a valid array of integer", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                } else if constexpr (std::is_floating_point_v<T>) {
                    if (!node->is_floating_point()) {
                        auto msg = std::format("'{}' is not a valid array of float", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (!node->is_string()) {
                        auto msg = std::format("'{}' is not a valid array of string", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                }

                auto msg = std::format("Invalid '{}'", a_key);
                throw toml::parse_error(msg.c_str(), node->source());
            }

            if (auto [ok, msg] = a_validator(*value); !ok) {
                throw toml::parse_error(msg.c_str(), node->source());
            }
            a_target.push_back(*std::move(value));
        }
    }

    template <Scalar T, class V = Validator<T>, bool required = false>
    inline void GetValue(const toml::table* a_table, std::string_view a_key, std::vector<T>& a_target,
        V&& a_validator = V())
    {
        return GetValue<T, V, required>(*a_table, a_key, a_target, std::forward<V>(a_validator));
    }

    template <Scalar T, class V = Validator<T>>
    inline void GetValueRequired(const toml::table& a_table, std::string_view a_key, T& a_target, V&& a_validator = V())
    {
        return GetValue<T, V, true>(a_table, a_key, a_target, std::forward<V>(a_validator));
    }

    template <Scalar T, class V = Validator<T>>
    inline void GetValueRequired(const toml::table* a_table, std::string_view a_key, T& a_target, V&& a_validator = V())
    {
        return GetValue<T, V, true>(*a_table, a_key, a_target, std::forward<V>(a_validator));
    }

    template <Scalar T, class V = Validator<T>>
    inline void GetValueRequired(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target,
        V&& a_validator = V())
    {
        return GetValue<T, V, true>(a_table, a_key, a_target, std::forward<V>(a_validator));
    }

    template <Scalar T, class V = Validator<T>>
    inline void GetValueRequired(const toml::table* a_table, std::string_view a_key, std::vector<T>& a_target,
        V&& a_validator = V())
    {
        return GetValue<T, V, true>(*a_table, a_key, a_target, std::forward<V>(a_validator));
    }

    template <Scalar T>
    inline void SetValue(toml::table& a_table, std::string_view a_key, const T& a_source)
    {
        auto [pos, ok] = a_table.emplace(a_key, a_source);
        if (!ok) {
            throw Error(std::format("'{}' exists", a_key));
        }
    }

    template <Scalar T>
    inline void SetValue(toml::table& a_table, std::string_view a_key, const std::vector<T>& a_source)
    {
        toml::array arr;
        arr.reserve(a_source.size());
        for (const auto& ele : a_source) {
            arr.push_back(ele);
        }

        auto [pos, ok] = a_table.emplace(a_key, std::move(arr));
        if (!ok) {
            throw Error(std::format("'{}' exists", a_key));
        }
    }
}
