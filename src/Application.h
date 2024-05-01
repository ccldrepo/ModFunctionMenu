#pragma once

#include <PCH.h>

#include "Util/Singleton.h"

enum class MFMEntryType : std::uint32_t
{
    kRegular = 0,
    kDirectory = 1,
};

class MFMEntry
{
public:
    MFMEntry(std::filesystem::path a_path, MFMEntryType a_type) :
        path(std::move(a_path)), name(PathToStr(path)), type(a_type)
    {
        BuildChild();
    }

    friend bool operator==(const MFMEntry& lhs, const MFMEntry& rhs) noexcept { return lhs.path == rhs.path; }

    friend std::strong_ordering operator<=>(const MFMEntry& lhs, const MFMEntry& rhs) noexcept
    {
        return lhs.path <=> rhs.path;
    }

private:
    void BuildChild();

public:
    std::filesystem::path path;
    std::string           name;
    MFMEntryType          type;
    std::vector<MFMEntry> children;
};

class MFMRoot
{
public:
    explicit MFMRoot(std::filesystem::path a_root) : root(std::move(a_root), MFMEntryType::kDirectory)
    {
        CurrentRoot(root);
    }

    MFMEntry* CurrentRoot() { return currentRoot; }
    void      CurrentRoot(MFMEntry& a_root) { CurrentRoot(std::addressof(a_root)); }
    void      CurrentRoot(MFMEntry* a_root)
    {
        currentRoot = a_root;
        currentRootStr = PathToStr(a_root->path).substr(39);
    }

    const std::string& CurrentRootStr() const { return currentRootStr; }

private:
    MFMEntry    root;
    MFMEntry*   currentRoot;
    std::string currentRootStr;
};

class Application final : public Singleton<Application>
{
    friend class Singleton<Application>;

public:
    //static bool IsMenu(std::string_view a_menuName);

private:
    Application() = default;

public:
    MFMRoot modRoot{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Mod/"sv };
    MFMRoot configRoot{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Config/"sv };

    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/"sv };
};
