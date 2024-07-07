#include "StringTable.h"

#include <XSEPlugin/Base/Translation.h>

namespace
{
    std::uint32_t transVersion = 0;
}

void StringTable::CheckReload()
{
    if (!Translation::IsVersionChanged(transVersion)) {
        return;
    }

    auto lock = Translation::LockShared();
    auto trans = Translation::GetSingleton();

    Title = trans->Lookup("$Title"sv);
    Section_Mod = trans->Lookup("$Section_Mod"sv);
    Section_Config = trans->Lookup("$Section_Config"sv);

    transVersion = Translation::LatestVersion();
    SKSE::log::debug("StringTable: Upgrade to transVersion {}.", transVersion);
}
