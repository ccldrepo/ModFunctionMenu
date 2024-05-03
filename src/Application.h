#pragma once

#include <PCH.h>

#include "Core.h"
#include "Util/Singleton.h"

class Application final : public Singleton<Application>
{
    friend class Singleton<Application>;

public:
    MFM_Tree* CurrentSection() noexcept { return currentSection; }
    void      CurrentSection(MFM_Tree& a_tree) { CurrentSection(std::addressof(a_tree)); }
    void      CurrentSection(MFM_Tree* a_tree) { currentSection = a_tree; }

    void ResetCurrentSection() { CurrentSection(modTree); }

private:
    Application() { ResetCurrentSection(); };

    MFM_Tree  modTree{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Mod/"sv };
    MFM_Tree  configTree{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Config/"sv };
    MFM_Tree* currentSection;

    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/"sv };
};
