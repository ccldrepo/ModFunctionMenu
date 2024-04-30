#include "HotkeyManager.h"

#include "CLib/Key.h"
#include "Configuration.h"
#include "Menu.h"

namespace
{
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
                auto key = CLib::ParseKeyCode(a_button->GetIDCode(), a_button->GetDevice());

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
            }
        }

    private:
        CLib::KeyCombo hotkey;
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
                auto key = CLib::ParseKeyCode(a_button->GetIDCode(), a_button->GetDevice());

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
            }
        }

    private:
        CLib::KeyCombo hotkey;
        CLib::Key      esc{ REX::W32::DIK_ESCAPE };
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
