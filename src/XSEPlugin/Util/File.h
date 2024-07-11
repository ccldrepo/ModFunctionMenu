#pragma once

#include <filesystem>
#include <format>
#include <stdexcept>

class FileError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

inline void CheckDirectory(const std::filesystem::path& a_path)
{
    auto st = std::filesystem::status(a_path);

    if (!std::filesystem::exists(st)) {
        // TODO: Remove PathToStr call in C++26.
        throw FileError(std::format("'{}' does not exist", PathToStr(a_path)));
    }

    if (!std::filesystem::is_directory(st)) {
        // TODO: Remove PathToStr call in C++26.
        throw FileError(std::format("'{}' is not a directory", PathToStr(a_path)));
    }
}

inline void CheckRegularFile(const std::filesystem::path& a_path)
{
    auto st = std::filesystem::status(a_path);

    if (!std::filesystem::exists(st)) {
        // TODO: Remove PathToStr call in C++26.
        throw FileError(std::format("'{}' does not exist", PathToStr(a_path)));
    }

    if (!std::filesystem::is_regular_file(st)) {
        // TODO: Remove PathToStr call in C++26.
        throw FileError(std::format("'{}' is not a regular file", PathToStr(a_path)));
    }
}
