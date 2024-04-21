#pragma once

#include <PCH.h>

namespace ImGui
{
    void Install();

    inline std::atomic<bool> initialized{ false };
    inline std::atomic<bool> shouldRender{ false };
}
