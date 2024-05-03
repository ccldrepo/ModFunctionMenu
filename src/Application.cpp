#include "Application.h"

void MFMNode::BuildChildren()
{
    if (type != Type::kDirectory) {
        return;
    }

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
