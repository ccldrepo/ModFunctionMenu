#pragma once

#include <PCH.h>

namespace ImGui
{
    class Renderer
    {
    public:
        static void Install();

        static bool Initialized() noexcept { return _initialized.load(); }
        static void Initialized(bool a_initialized) noexcept { _initialized.store(a_initialized); }
        static bool ShouldRender() noexcept { return _shouldRender.load(); }
        static void ShouldRender(bool a_shouldRender) noexcept { _shouldRender.store(a_shouldRender); }

    private:
        static inline std::atomic<bool> _initialized{ false };
        static inline std::atomic<bool> _shouldRender{ false };
    };
}
