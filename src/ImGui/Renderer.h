#pragma once

namespace ImGui
{
    class Renderer
    {
    public:
        static void Install();

        static void Init();
        static void Run();

        static bool IsInit() noexcept { return _isInit.load(); }
        static bool IsEnable() noexcept { return _isEnable.load(); }

        static void Enable() noexcept;
        static void Disable() noexcept;

    private:
        static inline std::atomic<bool> _isInit{ false };
        static inline std::atomic<bool> _isEnable{ false };
    };
}
