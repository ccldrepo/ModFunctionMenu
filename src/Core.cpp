#include "Core.h"

#include "Util/TOML.h"
#include "Util/Win.h"

MFM_Function MFM_Function::Get(const std::filesystem::path& a_path)
{
    MFM_Function func;

    auto data = LoadTOMLFile(a_path);

    GetTOMLValueRequired(data, "dll"sv, func.dll);

    GetTOMLValueRequired(data, "api"sv, func.api);

    std::string type;
    GetTOMLValue(data, "type"sv, type);
    func.type = MFMAPI_Type_StrToEnum(type);

    std::string preAction;
    GetTOMLValue(data, "preAction"sv, preAction);
    func.preAction = MFMAPI_PreAction_StrToEnum(preAction);

    std::string postAction;
    GetTOMLValue(data, "postAction"sv, postAction);
    func.postAction = MFMAPI_PostAction_StrToEnum(postAction);

    SKSE::log::info("Get function: dll = \"{}\", api = \"{}\", type = \"{}\", preAction = \"{}\", postAction = \"{}\".",
        func.dll, func.api, type, preAction, postAction);
    return func;
}

void MFM_Function::operator()() const
{
    auto dllPath = StrToPath(dll);
    auto func = Win::GetModuleFunc<MFMAPI_Void>(dllPath.c_str(), api.c_str());
    if (!func) {
        auto msg = std::format("Invalid function: dll = \"{}\", api = \"{}\"", dll, api);
        throw std::runtime_error(msg);
    }
    return func();
}

void MFM_Function::operator()(char* a_msg, std::size_t a_len) const
{
    auto dllPath = StrToPath(dll);
    auto func = Win::GetModuleFunc<MFMAPI_Message>(dllPath.c_str(), api.c_str());
    if (!func) {
        auto msg = std::format("Invalid function: dll = \"{}\", api = \"{}\"", dll, api);
        throw std::runtime_error(msg);
    }
    return func(a_msg, a_len);
}

MFM_Node::MFM_Node(std::filesystem::path a_path, Type a_type) : MFM_Node(std::move(a_path), a_type, this) {}

MFM_Node::MFM_Node(std::filesystem::path a_path, Type a_type, MFM_Node* a_parent) :
    path(std::move(a_path)), type(a_type), parent(a_parent)
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

void MFM_Node::BuildChildren()
{
    auto st = std::filesystem::status(path);

    if (!std::filesystem::exists(st)) {
        SKSE::log::warn("\"{}\" does not exist.", PathToStr(path));
        return;
    }

    if (!std::filesystem::is_directory(st)) {
        SKSE::log::error("\"{}\" is not a directory.", PathToStr(path));
        return;
    }

    for (const auto& entry : std::filesystem::directory_iterator{ path }) {
        if (entry.is_regular_file()) {
#pragma warning(push)
#pragma warning(disable: 4458)
            if (const auto& path = entry.path(); path.extension().native() == L".toml"sv) {
                children.emplace_back(path, Type::kRegular, this);
            }
#pragma warning(pop)
        } else if (entry.is_directory()) {
            children.emplace_back(entry.path(), Type::kDirectory, this);
        }
    }

    std::ranges::sort(children);
}
