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
        explicit MenuOpenHotkeyContext(const Configuration* a_config) :
            keyboard(a_config->controls.keyboard.iHotkey, a_config->controls.keyboard.iModifier),
            gamepad(a_config->controls.gamepad.iHotkey, a_config->controls.gamepad.iModifier)
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
                ImGui::Menu::GetSingleton()->Open();
            }
        }

    private:
        CLib::KeyCombo keyboard;
        CLib::KeyCombo gamepad;
    };

    class MenuCloseHotkeyContext
    {
    public:
        explicit MenuCloseHotkeyContext(const Configuration* a_config) :
            keyboard(a_config->controls.keyboard.iHotkey, a_config->controls.keyboard.iModifier),
            gamepad(a_config->controls.gamepad.iHotkey, a_config->controls.gamepad.iModifier),
            kbExtraExit(a_config->controls.keyboard.iExtraExit),  //
            gpExtraExit(a_config->controls.gamepad.iExtraExit)    //
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
                    kbExtraExit.Update(key);
                    gpExtraExit.Update(key);
                }
            }
        }

        void Finalize()
        {
            if (kbExtraExit.IsActive() || keyboard.IsActive() || gpExtraExit.IsActive() || gamepad.IsActive()) {
                ImGui::Menu::GetSingleton()->Close();
            }
        }

    private:
        CLib::KeyCombo keyboard;
        CLib::KeyCombo gamepad;
        CLib::Key      kbExtraExit;
        CLib::Key      gpExtraExit;
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
