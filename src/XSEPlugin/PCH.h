#pragma once

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <bit>
#include <bitset>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <chrono>
#include <climits>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <execution>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <ios>
#include <istream>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <ranges>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <version>

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <REX/PS4.h>
#include <REX/W32.h>
#include <SKSE/SKSE.h>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

using namespace std::literals::string_view_literals;

namespace SKSE::stl
{
    [[nodiscard]] inline auto narrow_to_wide(std::uint32_t a_cp, std::string_view a_in) noexcept
        -> std::optional<std::wstring>
    {
        const auto cvt = [&](wchar_t* a_dst, std::size_t a_length) {
            return REX::W32::MultiByteToWideChar(a_cp, 0, a_in.data(), static_cast<int>(a_in.length()), a_dst,
                static_cast<int>(a_length));
        };

        const auto len = cvt(nullptr, 0);
        if (len == 0) {
            return std::nullopt;
        }

        std::wstring out(len, '\0');
        if (cvt(out.data(), out.length()) == 0) {
            return std::nullopt;
        }
        return out;
    }

    [[nodiscard]] inline auto wide_to_narrow(std::uint32_t a_cp, std::wstring_view a_in) noexcept
        -> std::optional<std::string>
    {
        const auto cvt = [&](char* a_dst, std::size_t a_length) {
            return REX::W32::WideCharToMultiByte(a_cp, 0, a_in.data(), static_cast<int>(a_in.length()), a_dst,
                static_cast<int>(a_length), nullptr, nullptr);
        };

        const auto len = cvt(nullptr, 0);
        if (len == 0) {
            return std::nullopt;
        }

        std::string out(len, '\0');
        if (cvt(out.data(), out.length()) == 0) {
            return std::nullopt;
        }
        return out;
    }

    [[nodiscard]] inline auto utf8_to_ansi(std::string_view a_in) noexcept -> std::optional<std::string>
    {
        if (auto wstr = utf8_to_utf16(a_in)) {
            return wide_to_narrow(std::to_underlying(__std_fs_code_page()), *wstr);
        }
        return std::nullopt;
    }

    [[nodiscard]] inline auto ansi_to_utf8(std::string_view a_in) noexcept -> std::optional<std::string>
    {
        if (auto wstr = narrow_to_wide(std::to_underlying(__std_fs_code_page()), a_in)) {
            return utf16_to_utf8(*wstr);
        }
        return std::nullopt;
    }

    inline void report_success(const std::string& a_msg, std::source_location a_loc = std::source_location::current())
    {
        spdlog::source_loc loc{ a_loc.file_name(), static_cast<int>(a_loc.line()), a_loc.function_name() };
        spdlog::log(loc, spdlog::level::info, a_msg);
        if (auto logger = spdlog::get("Base")) {
            logger->log(loc, spdlog::level::info, a_msg);
        }
    }

    [[noreturn]] inline void report_failure(const std::string& a_msg, bool a_abort,
        std::source_location a_loc = std::source_location::current())
    {
        // Abort or throw when error occurred.
        if (a_abort) {
            report_and_fail(a_msg, a_loc);
        } else {
            spdlog::source_loc loc{ a_loc.file_name(), static_cast<int>(a_loc.line()), a_loc.function_name() };
            spdlog::log(loc, spdlog::level::err, a_msg);
            if (auto logger = spdlog::get("Base")) {
                logger->log(loc, spdlog::level::err, a_msg);
            }
            throw std::runtime_error(a_msg);
        }
    }
}

[[nodiscard]] inline std::filesystem::path StrToPath(std::string_view a_str)
{
    if (auto wstr = SKSE::stl::utf8_to_utf16(a_str)) {
        return std::filesystem::path{ *std::move(wstr) };
    }
    return {};
}

[[nodiscard]] inline std::string PathToStr(const std::filesystem::path& a_path)
{
    if (auto str = SKSE::stl::utf16_to_utf8(a_path.native())) {
        return *std::move(str);
    }
    return {};
}

inline void ReconfigureLogger(std::string_view a_logLevel)
{
    if (a_logLevel.empty()) {
#ifndef _DEBUG
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);
#else
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::trace);
#endif
    } else if (a_logLevel == "trace"sv) {
        spdlog::set_level(spdlog::level::trace);
        spdlog::flush_on(spdlog::level::trace);
    } else if (a_logLevel == "debug"sv) {
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_on(spdlog::level::debug);
    } else if (a_logLevel == "info"sv) {
        spdlog::set_level(spdlog::level::info);
        spdlog::flush_on(spdlog::level::info);
    } else if (a_logLevel == "warn"sv) {
        spdlog::set_level(spdlog::level::warn);
        spdlog::flush_on(spdlog::level::warn);
    } else if (a_logLevel == "error"sv) {
        spdlog::set_level(spdlog::level::err);
        spdlog::flush_on(spdlog::level::err);
    } else if (a_logLevel == "critical"sv) {
        spdlog::set_level(spdlog::level::critical);
        spdlog::flush_on(spdlog::level::critical);
    } else {
        ReconfigureLogger(""sv);
        spdlog::warn("Invalid log level: {}.", a_logLevel);
    }
}
