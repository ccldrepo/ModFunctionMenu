#include "Core.h"

#include "Util/TOML.h"
#include "Util/Win.h"

MFM_Function MFM_Function::Get(const std::filesystem::path& a_path)
{
    MFM_Function func;

    auto data = LoadTOMLFile(a_path);

    LoadTOMLValue(data, "dll"sv, func.dll);
    if (func.dll.empty()) {
        throw TOMLError("'dll' is required");
    }

    LoadTOMLValue(data, "api"sv, func.api);
    if (func.api.empty()) {
        throw TOMLError("'api' is required");
    }

    std::string type;
    LoadTOMLValue(data, "type"sv, type);
    func.type = MFMAPI_Type_StrToEnum(type);

    std::string preAction;
    LoadTOMLValue(data, "preAction"sv, preAction);
    func.preAction = MFMAPI_PreAction_StrToEnum(preAction);

    std::string postAction;
    LoadTOMLValue(data, "postAction"sv, postAction);
    func.postAction = MFMAPI_PostAction_StrToEnum(postAction);

    return func;
}

void MFM_Function::operator()()
{
    auto dllPath = StrToPath(dll);
    auto func = GetModuleFunc<MFMAPI_Void>(dllPath.c_str(), api.c_str());
    if (!func) {
        auto msg = std::format("Invalid function: dll = \"{}\", api = \"{}\"", dll, api);
        throw std::runtime_error(msg);
    }
    return func();
}

void MFM_Function::operator()(char* a_msg, std::size_t a_len)
{
    auto dllPath = StrToPath(dll);
    auto func = GetModuleFunc<MFMAPI_Message>(dllPath.c_str(), api.c_str());
    if (!func) {
        auto msg = std::format("Invalid function: dll = \"{}\", api = \"{}\"", dll, api);
        throw std::runtime_error(msg);
    }
    return func(a_msg, a_len);
}

MFM_Node::MFM_Node(std::filesystem::path a_path, Type a_type) : path(std::move(a_path)), type(a_type)
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
    for (const auto& entry : std::filesystem::directory_iterator{ path }) {
        if (entry.is_regular_file()) {
#pragma warning(push)
#pragma warning(disable: 4458)
            if (const auto& path = entry.path(); path.extension().native() == L".toml"sv) {
                children.emplace_back(path, Type::kRegular);
            }
#pragma warning(pop)
        } else if (entry.is_directory()) {
            children.emplace_back(entry.path(), Type::kDirectory);
        }
    }

    std::ranges::sort(children);
}
