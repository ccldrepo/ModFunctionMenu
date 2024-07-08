#include "StringTable.h"

#include <XSEPlugin/Base/Translation.h>

void StringTable::CheckReload()
{
    if (!Translation::IsVersionChanged(_transVersion)) {
        return;
    }

    auto lock = Translation::LockShared();
    auto trans = Translation::GetSingleton();

    Title = trans->Lookup("$Title"sv);
    Section_Mod = trans->Lookup("$Section_Mod"sv);
    Section_Config = trans->Lookup("$Section_Config"sv);

    _transVersion = Translation::GetVersion();
    SKSE::log::debug("StringTable: Upgrade to Translation Version {}.", _transVersion);
}

StringTable StringTable::_singleton;
