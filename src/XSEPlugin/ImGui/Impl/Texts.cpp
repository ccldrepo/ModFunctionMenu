#include "Texts.h"

#include <XSEPlugin/Base/Translation.h>

namespace ImGui::Impl
{
    void Texts::Load()
    {
        auto trans = Translation::GetSingleton();

        Title = trans->Lookup("$Title"sv);
        Section_Mod = trans->Lookup("$Section_Mod"sv);
        Section_Config = trans->Lookup("$Section_Config"sv);
    }
}
