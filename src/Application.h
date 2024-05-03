#pragma once

#include <PCH.h>

#include "Core.h"
#include "Util/Singleton.h"

class Application final : public Singleton<Application>
{
    friend class Singleton<Application>;

public:
    //static bool IsMenu(std::string_view a_menuName);

private:
    Application() = default;

public:
    MFM_Tree modTree{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Mod/"sv };
    MFM_Tree configTree{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/Config/"sv };

    static inline const std::filesystem::path root{ L"Data/SKSE/Plugins/ccld_ModFunctionMenu/"sv };
};
