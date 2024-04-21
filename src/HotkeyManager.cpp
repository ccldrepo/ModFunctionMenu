#include "HotkeyManager.h"

#include "Configuration.h"
#include "Menu.h"

namespace
{
    inline std::uint32_t RemapKey(std::uint32_t a_key, RE::INPUT_DEVICE a_device)
    {
        switch (a_device) {
        case RE::INPUT_DEVICE::kKeyboard:
            break;
        case RE::INPUT_DEVICE::kMouse:
            a_key += SKSE::InputMap::kMacro_MouseButtonOffset;
            break;
        case RE::INPUT_DEVICE::kGamepad:
            a_key = SKSE::InputMap::GamepadMaskToKeycode(a_key);
            break;
        default:
            break;
        }
        return a_key;
    }

    class Key
    {
    public:
        constexpr explicit Key(std::uint32_t a_targetHotkey) noexcept : targetHotkey(a_targetHotkey) {}

        bool IsActive() const noexcept  //
        {
            return hasHotkey;
        }

        void Update(std::uint32_t a_key) noexcept
        {
            if (targetHotkey != 0 && a_key == targetHotkey) {
                hasHotkey = true;
            }
        }

    private:
        const std::uint32_t targetHotkey;

        bool hasHotkey{ false };
    };

    class KeyCombo
    {
    public:
        constexpr KeyCombo(std::uint32_t a_targetHotkey, std::uint32_t a_targetModifier) noexcept :
            targetHotkey(a_targetHotkey), targetModifier(a_targetModifier),
            count(CalcCount(a_targetHotkey, a_targetModifier))
        {}

        bool IsActive() const noexcept  //
        {
            return (hasHotkey && targetModifier == 0) || (hasHotkey && hasModifier);
        }

        void UpdateDown(std::uint32_t a_key) noexcept
        {
            if (targetHotkey != 0 && a_key == targetHotkey) {
                hasHotkey = true;
            }
        }

        void UpdatePressed(std::uint32_t a_key) noexcept
        {
            if (targetModifier != 0 && a_key == targetModifier) {
                hasModifier = true;
            }
        }

    private:
        static constexpr uint16_t CalcCount(std::uint32_t a_targetHotkey, std::uint32_t a_targetModifier) noexcept
        {
            if (a_targetHotkey == 0) {
                return 0;
            } else if (a_targetModifier == 0) {
                return 1;
            } else {
                return 2;
            }
        }

        const std::uint32_t targetHotkey;
        const std::uint32_t targetModifier;

        bool hasHotkey{ false };
        bool hasModifier{ false };

        const uint16_t count;
    };

    class MenuOpenHotkeyContext
    {
    public:
        explicit MenuOpenHotkeyContext(const Configuration* config) : hotkey(config->iHotkey, config->iModifier) {}

        void Update(const RE::ButtonEvent* a_button)
        {
            if (!a_button->HasIDCode()) {
                return;
            }

            if (a_button->IsPressed()) {
                auto key = RemapKey(a_button->GetIDCode(), a_button->GetDevice());

                hotkey.UpdatePressed(key);

                if (a_button->IsDown()) {
                    hotkey.UpdateDown(key);
                }
            }
        }

        void Finalize(Menu* menu)
        {
            if (hotkey.IsActive()) {
                menu->Open();
                HotkeyManager::shouldBlockInput = true;
            }
        }

    private:
        KeyCombo hotkey;
    };

    class MenuCloseHotkeyContext
    {
    public:
        explicit MenuCloseHotkeyContext(const Configuration* config) : hotkey(config->iHotkey, config->iModifier) {}

        void Update(const RE::ButtonEvent* a_button)
        {
            if (!a_button->HasIDCode()) {
                return;
            }

            if (a_button->IsPressed()) {
                auto key = RemapKey(a_button->GetIDCode(), a_button->GetDevice());

                hotkey.UpdatePressed(key);

                if (a_button->IsDown()) {
                    hotkey.UpdateDown(key);
                    esc.Update(key);
                }
            }
        }

        void Finalize(Menu* menu)
        {
            if (esc.IsActive() || hotkey.IsActive()) {
                menu->Close();
                HotkeyManager::shouldBlockInput = false;
            }
        }

    private:
        KeyCombo hotkey;
        Key      esc{ REX::W32::DIK_ESCAPE };
    };

    template <class HotkeyContext>
    void ProcessMenuOpenClose(const RE::InputEvent* const* a_event, Menu* menu)
    {
        const auto config = Configuration::GetSingleton();

        HotkeyContext ctx{ config };

        for (auto event = *a_event; event; event = event->next) {
            if (auto button = event->AsButtonEvent()) {
                ctx.Update(button);
            }
        }

        ctx.Finalize(menu);
    }
}

void HotkeyManager::Process(const RE::InputEvent* const* a_event)
{
    auto menu = Menu::GetSingleton();
    if (!menu->IsOpen()) {
        ProcessMenuOpenClose<MenuOpenHotkeyContext>(a_event, menu);
    } else {
        ProcessMenuOpenClose<MenuCloseHotkeyContext>(a_event, menu);
    }
}
