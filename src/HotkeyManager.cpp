#include "HotkeyManager.h"

#include "CLib/Key.h"
#include "Configuration.h"

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
                auto key = CLib::ParseKey(a_button->GetIDCode(), a_button->GetDevice());

                hotkey.UpdatePressed(key);

                if (a_button->IsDown()) {
                    hotkey.UpdateDown(key);
                }
            }
        }

        void Finalize(Menu* a_menu)
        {
            if (hotkey.IsActive()) {
                a_menu->Open();
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
                auto key = CLib::ParseKey(a_button->GetIDCode(), a_button->GetDevice());

                hotkey.UpdatePressed(key);

                if (a_button->IsDown()) {
                    hotkey.UpdateDown(key);
                    esc.Update(key);
                }
            }
        }

        void Finalize(Menu* a_menu)
        {
            if (esc.IsActive() || hotkey.IsActive()) {
                a_menu->Close();
            }
        }

    private:
        CLib::KeyCombo hotkey;
        CLib::Key      esc{ REX::W32::DIK_ESCAPE };
    };

    template <class HotkeyContext>
    void ProcessMenuOpenClose(const RE::InputEvent* const* a_event, Menu* a_menu)
    {
        const auto config = Configuration::GetSingleton();

        HotkeyContext ctx{ config };

        for (auto event = *a_event; event; event = event->next) {
            if (auto button = event->AsButtonEvent()) {
                ctx.Update(button);
            }
        }

        ctx.Finalize(a_menu);
    }
}

void HotkeyManager::ProcessMenuOpen(const RE::InputEvent* const* a_event, Menu* a_menu)
{
    ProcessMenuOpenClose<MenuOpenHotkeyContext>(a_event, a_menu);
}

void HotkeyManager::ProcessMenuClose(const RE::InputEvent* const* a_event, Menu* a_menu)
{
    ProcessMenuOpenClose<MenuCloseHotkeyContext>(a_event, a_menu);
}
