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

    MFM_Tree  modTree{ MFM_Path::mod };
    MFM_Tree  configTree{ MFM_Path::config };
    MFM_Tree* currentSection;
};
