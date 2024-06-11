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

        void Finalize()
        {
            if (keyboard.IsActive() || gamepad.IsActive()) {
                Menu::GetSingleton()->Open();
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

        void Finalize()
        {
            if (esc.IsActive() || keyboard.IsActive() || gamepad.IsActive()) {
                Menu::GetSingleton()->Close();
            }
        }

    private:
        CLib::KeyCombo keyboard;
        CLib::KeyCombo gamepad;
        CLib::Key      esc{ REX::W32::DIK_ESCAPE };
    };

    template <class HotkeyContext>
    void ProcessHotkey(const RE::InputEvent* const* a_event)
    {
        HotkeyContext ctx{ Configuration::GetSingleton() };
        for (auto event = *a_event; event; event = event->next) {
            if (auto button = event->AsButtonEvent()) {
                ctx.Update(button);
            }
        }
        ctx.Finalize();
    }
}

void InputManager::Process(const RE::InputEvent* const* a_event)
{
    if (InputBlocker::IsNotBlocked()) {
        ProcessHotkey<MenuOpenHotkeyContext>(a_event);
    } else {
        ImGui::TranslateInputEvent(a_event);
        ProcessHotkey<MenuCloseHotkeyContext>(a_event);
    }
}

void InputManager::Cleanup() { ImGui::ClearInputEvent(); }
