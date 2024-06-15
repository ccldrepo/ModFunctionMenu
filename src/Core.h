#pragma once

#include "Function.h"
#include "Util/Singleton.h"

struct MFM_Path
{
    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/"sv };

    static inline const std::filesystem::path mod{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Mod"sv };
    static inline const std::filesystem::path config{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Config"sv };
};

struct MFM_Function
{
    [[nodiscard]] static MFM_Function Get(const std::filesystem::path& a_path);

    void operator()() const;
    void operator()(char* a_msg, std::size_t a_len) const;

    std::string       dll;
    std::string       api;
    MFMAPI_Type       type{ MFMAPI_Type::kVoid };
    MFMAPI_PreAction  preAction{ MFMAPI_PreAction::kNone };
    MFMAPI_PostAction postAction{ MFMAPI_PostAction::kNone };
};

class MFM_Node
{
public:
    enum class Type : std::uint32_t
    {
        kRegular = 0,
        kDirectory = 1,
    };

    MFM_Node(const std::filesystem::path& a_path, Type a_type);
    MFM_Node(const std::filesystem::path& a_path, Type a_type, MFM_Node* a_parent);

    friend bool operator==(const MFM_Node& a_lhs, const MFM_Node& a_rhs) noexcept { return a_lhs.path == a_rhs.path; }

    friend std::strong_ordering operator<=>(const MFM_Node& a_lhs, const MFM_Node& a_rhs) noexcept
    {
        return a_lhs.path <=> a_rhs.path;
    }

private:
    void BuildChildren();

public:
    std::filesystem::path                  path;
    std::string                            name;
    Type                                   type;
    std::vector<std::unique_ptr<MFM_Node>> children;
    MFM_Node*                              parent;
};

class MFM_Tree
{
public:
    explicit MFM_Tree(std::filesystem::path a_root) : root(std::move(a_root), MFM_Node::Type::kDirectory)
    {
        ResetCurrentPath();
    }

    const MFM_Node* CurrentPath() const noexcept { return currentPath; }
    void            CurrentPath(const MFM_Node& a_node) { CurrentPath(std::addressof(a_node)); }
    void            CurrentPath(const MFM_Node* a_node)
    {
        currentPath = a_node;
        currentPathStr = PathToStr(a_node->path).substr(MFM_Path::root.native().size() - 1);
    }

    const std::string& CurrentPathStr() const noexcept { return currentPathStr; }

    void ResetCurrentPath() { CurrentPath(root); }

    void ResetCurrentPathToParent() { CurrentPath(currentPath->parent); }

private:
    const MFM_Node  root;
    const MFM_Node* currentPath;
    std::string     currentPathStr;
};

class Datastore final : public Singleton<Datastore>
{
    friend class Singleton<Datastore>;

public:
    MFM_Tree* CurrentSection() noexcept { return currentSection; }
    void      CurrentSection(MFM_Tree& a_tree) { CurrentSection(std::addressof(a_tree)); }
    void      CurrentSection(MFM_Tree* a_tree) { currentSection = a_tree; }

    void ResetCurrentSection() { CurrentSection(modTree); }

    MFM_Tree  modTree{ MFM_Path::mod };
    MFM_Tree  configTree{ MFM_Path::config };
    MFM_Tree* currentSection;

private:
    Datastore() { ResetCurrentSection(); };
};
