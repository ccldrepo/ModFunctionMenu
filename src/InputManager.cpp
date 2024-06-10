#include "InputManager.h"

#include "CLib/Key.h"
#include "Configuration.h"
#include "ImGui/Input.h"
#include "ImGui/Menu.h"

namespace
{
    class MenuOpenHotkeyContext
    {
    public:
        explicit MenuOpenHotkeyContext(const Configuration* config) :
            keyboard(config->controls.keyboard.iHotkey, config->controls.keyboard.iModifier),
            gamepad(config->controls.gamepad.iHotkey, config->controls.gamepad.iModifier)
        {}

        void Update(const RE::ButtonEvent* a_button)
        {
            if (!a_button->HasIDCode()) {
                return;
            }

            if (a_button->IsPressed()) {
                auto key = CLib::ParseKey(a_button->GetIDCode(), a_button->GetDevice());

                keyboard.UpdatePressed(key);
                gamepad.UpdatePressed(key);

                if (a_button->IsDown()) {
                    keyboard.UpdateDown(key);
                    gamepad.UpdateDown(key);
                }
            }
        }

        void Finalize(Menu* a_menu)
        {
            if (keyboard.IsActive() || gamepad.IsActive()) {
                a_menu->Open();
            }
        }

    private:
        CLib::KeyCombo keyboard;
        CLib::KeyCombo gamepad;
    };

    class MenuCloseHotkeyContext
    {
    public:
        explicit MenuCloseHotkeyContext(const Configuration* config) :
            keyboard(config->controls.keyboard.iHotkey, config->controls.keyboard.iModifier),
            gamepad(config->controls.gamepad.iHotkey, config->controls.gamepad.iModifier)
        {}

        void Update(const RE::ButtonEvent* a_button)
        {
            if (!a_button->HasIDCode()) {
                return;
            }

            if (a_button->IsPressed()) {
                auto key = CLib::ParseKey(a_button->GetIDCode(), a_button->GetDevice());

                keyboard.UpdatePressed(key);
                gamepad.UpdatePressed(key);

                if (a_button->IsDown()) {
                    keyboard.UpdateDown(key);
                    gamepad.UpdateDown(key);
                    esc.Update(key);
                }
            }
        }

        void Finalize(Menu* a_menu)
        {
            if (esc.IsActive() || keyboard.IsActive() || gamepad.IsActive()) {
                a_menu->Close();
            }
        }

    private:
        CLib::KeyCombo keyboard;
        CLib::KeyCombo gamepad;
        CLib::Key      esc{ REX::W32::DIK_ESCAPE };
    };

    template <class HotkeyContext>
    void ProcessMenuOpenClose(const RE::InputEvent* const* a_event, Menu* a_menu)
    {
        HotkeyContext ctx{ Configuration::GetSingleton() };
        for (auto event = *a_event; event; event = event->next) {
            if (auto button = event->AsButtonEvent()) {
                ctx.Update(button);
            }
        }
        ctx.Finalize(a_menu);
    }
}

void InputManager::Process(const RE::InputEvent* const* a_event)
{
    auto menu = Menu::GetSingleton();
    if (!menu->IsOpen()) {
        ProcessMenuOpenClose<MenuOpenHotkeyContext>(a_event, menu);
    } else {
        // Forward input event to ImGui while menu is open.
        ImGui::TranslateInputEvent(a_event);
        ProcessMenuOpenClose<MenuCloseHotkeyContext>(a_event, menu);
    }
}

void InputManager::Cleanup()
{
    // Clear input buffer for ImGui.
    ImGui::ClearInputEvent();
}
