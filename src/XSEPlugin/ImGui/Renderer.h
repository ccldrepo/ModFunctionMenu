#pragma once

#include <XSEPlugin/ImGui/Impl/Fonts.h>
#include <XSEPlugin/ImGui/Impl/Styles.h>
#include <XSEPlugin/ImGui/Impl/Texts.h>

namespace ImGui
{
    class Renderer
    {
    public:
        [[nodiscard]] static Renderer* GetSingleton() noexcept { return std::addressof(_singleton); }

        static void Install();

        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void Init();
        void Run();

        [[nodiscard]] bool IsInit() const noexcept { return _isInit.load(); }
        [[nodiscard]] bool IsEnable() const noexcept { return _isEnable.load(); }

        void Enable() noexcept;
        void Disable() noexcept;

        Impl::Fonts  fonts;
        Impl::Styles styles;
        Impl::Texts  texts;

    private:
        Renderer() = default;

        ~Renderer() = default;

        void Load();

        static Renderer _singleton;

        std::atomic<bool> _isInit{ false };
        std::atomic<bool> _isEnable{ false };

        std::uint32_t _configVersion{ 0 };
        std::uint32_t _transVersion{ 0 };
    };
}
