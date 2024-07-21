#pragma once

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
            auto msg = std::format("'{}' is not a section", a_key);
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

    template <Scalar T, bool required = false>
    inline void GetValue(const toml::table& a_table, std::string_view a_key, T& a_target)
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
                    auto msg = std::format("'{}' is not a bool", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            } else if constexpr (std::is_integral_v<T>) {
                if (!node->is_integer()) {
                    auto msg = std::format("'{}' is not an integer", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            } else if constexpr (std::is_floating_point_v<T>) {
                if (!node->is_floating_point()) {
                    auto msg = std::format("'{}' is not a float", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (!node->is_string()) {
                    auto msg = std::format("'{}' is not a string", a_key);
                    throw toml::parse_error(msg.c_str(), node->source());
                }
            }

            auto msg = std::format("Invalid '{}'", a_key);
            throw toml::parse_error(msg.c_str(), node->source());
        }
        a_target = *std::move(value);
    }

    template <Scalar T, bool required = false>
    inline void GetValue(const toml::table* a_table, std::string_view a_key, T& a_target)
    {
        return GetValue<T, required>(*a_table, a_key, a_target);
    }

    template <Scalar T, bool required = false>
    inline void GetValue(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target)
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
            auto msg = std::format("'{}' is not an array", a_key);
            throw toml::parse_error(msg.c_str(), node->source());
        }

        a_target.clear();
        a_target.reserve(arr->size());
        for (const auto& ele : *arr) {
            auto value = ele.value<T>();
            if (!value) {
                if constexpr (std::is_same_v<T, bool>) {
                    if (!node->is_boolean()) {
                        auto msg = std::format("'{}' is not an array of bool", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                } else if constexpr (std::is_integral_v<T>) {
                    if (!node->is_integer()) {
                        auto msg = std::format("'{}' is not an array of integer", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                } else if constexpr (std::is_floating_point_v<T>) {
                    if (!node->is_floating_point()) {
                        auto msg = std::format("'{}' is not an array of float", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (!node->is_string()) {
                        auto msg = std::format("'{}' is not an array of string", a_key);
                        throw toml::parse_error(msg.c_str(), node->source());
                    }
                }

                auto msg = std::format("Invalid '{}'", a_key);
                throw toml::parse_error(msg.c_str(), node->source());
            }
            a_target.push_back(*std::move(value));
        }
    }

    template <Scalar T, bool required = false>
    inline void GetValue(const toml::table* a_table, std::string_view a_key, std::vector<T>& a_target)
    {
        return GetValue<T, required>(*a_table, a_key, a_target);
    }

    template <Scalar T>
    inline void GetValueRequired(const toml::table& a_table, std::string_view a_key, T& a_target)
    {
        return GetValue<T, true>(a_table, a_key, a_target);
    }

    template <Scalar T>
    inline void GetValueRequired(const toml::table* a_table, std::string_view a_key, T& a_target)
    {
        return GetValue<T, true>(*a_table, a_key, a_target);
    }

    template <Scalar T>
    inline void GetValueRequired(const toml::table& a_table, std::string_view a_key, std::vector<T>& a_target)
    {
        return GetValue<T, true>(a_table, a_key, a_target);
    }

    template <Scalar T>
    inline void GetValueRequired(const toml::table* a_table, std::string_view a_key, std::vector<T>& a_target)
    {
        return GetValue<T, true>(*a_table, a_key, a_target);
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
