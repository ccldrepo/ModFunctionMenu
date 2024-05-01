#include "Application.h"

void MFMEntry::BuildChild()
{
    if (type != MFMEntryType::kDirectory) {
        return;
    }

    for (auto& entry : std::filesystem::directory_iterator{ path }) {
        if (entry.is_regular_file()) {
#pragma warning(push)
#pragma warning(disable: 4458)
            if (auto& path = entry.path(); path.extension().native() == L".toml"sv) {
                children.emplace_back(path, MFMEntryType::kRegular);
            }
#pragma warning(pop)
        } else if (entry.is_directory()) {
            children.emplace_back(entry.path(), MFMEntryType::kDirectory);
        }
    }

    std::ranges::sort(children);
}
