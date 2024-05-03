#pragma once

#include <PCH.h>

#include "Util/Singleton.h"

class MFMNode
{
public:
    enum class Type : std::uint32_t
    {
        kRegular = 0,
        kDirectory = 1,
    };

    MFMNode(std::filesystem::path a_path, Type a_type) : path(std::move(a_path)), type(a_type)
    {
        switch (type) {
        case Type::kRegular:
            name = PathToStr(path.stem());
            break;
        case Type::kDirectory:
            name = PathToStr(path.filename());
            BuildChildren();
            break;
        }
    }

    friend bool operator==(const MFMNode& lhs, const MFMNode& rhs) noexcept { return lhs.path == rhs.path; }

    friend std::strong_ordering operator<=>(const MFMNode& lhs, const MFMNode& rhs) noexcept
    {
        return lhs.path <=> rhs.path;
    }

private:
    void BuildChildren();

public:
    std::filesystem::path path;
    std::string           name;
    Type                  type;
    std::vector<MFMNode>  children;
};

class MFMTree
{
public:
    explicit MFMTree(std::filesystem::path a_root) : root(std::move(a_root), MFMNode::Type::kDirectory)
    {
        ResetCurrentRoot();
    }

    const MFMNode* CurrentRoot() const noexcept { return currentRoot; }
    void           CurrentRoot(const MFMNode& a_root) { CurrentRoot(std::addressof(a_root)); }
    void           CurrentRoot(const MFMNode* a_root)
    {
        currentRoot = a_root;
        currentRootStr = PathToStr(a_root->path).substr(39);
    }

    const std::string& CurrentRootStr() const noexcept { return currentRootStr; }

    void ResetCurrentRoot() { CurrentRoot(root); }

private:
    const MFMNode  root;
    const MFMNode* currentRoot;
    std::string    currentRootStr;
};

class Application final : public Singleton<Application>
{
    friend class Singleton<Application>;

public:
    //static bool IsMenu(std::string_view a_menuName);

private:
    Application() = default;

public:
    MFMTree modTree{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Mod/"sv };
    MFMTree configTree{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Config/"sv };

    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/"sv };
};
