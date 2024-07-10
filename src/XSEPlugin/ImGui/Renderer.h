#pragma once

namespace ImGui
{
    class Renderer
    {
    public:
        static void Install();

        static void Init();
        static void Run();

        [[nodiscard]] static bool IsInit() noexcept { return _isInit.load(); }
        [[nodiscard]] static bool IsEnable() noexcept { return _isEnable.load(); }

        static void Enable() noexcept;
        static void Disable() noexcept;

    private:
        static inline std::atomic<bool> _isInit{ false };
        static inline std::atomic<bool> _isEnable{ false };

        static inline std::uint32_t _configVersion{ 0 };
        static inline std::uint32_t _transVersion{ 0 };
    };
}
