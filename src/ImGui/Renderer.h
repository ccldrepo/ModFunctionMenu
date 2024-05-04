#pragma once

#include <PCH.h>

namespace ImGui
{
    class Renderer
    {
    public:
        static void Install();

        static inline std::atomic<bool> initialized{ false };
        static inline std::atomic<bool> shouldRender{ false };
    };
}
