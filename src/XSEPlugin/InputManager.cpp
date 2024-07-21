#include "InputManager.h"

#include <XSEPlugin/Base/Configuration.h>
#include <XSEPlugin/ImGui/Input.h>
#include <XSEPlugin/ImGui/Menu.h>
#include <XSEPlugin/Util/CLib/Key.h>

namespace
{
    class MenuOpenHotkeyContext
    {
    public:
        void Load()
        {
            auto& cfgControls = Configuration::GetSingleton()->controls;

            keyboard = CLib::KeyCombo{ cfgControls.keyboard.iHotkey, cfgControls.keyboard.iModifier };
            gamepad = CLib::KeyCombo{ cfgControls.gamepad.iHotkey, cfgControls.gamepad.iModifier };
        }

        void Reset() noexcept
        {
            keyboard.Reset();
            gamepad.Reset();
        }

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
        void Load()
        {
            auto& cfgControls = Configuration::GetSingleton()->controls;

            keyboard = CLib::KeyCombo{ cfgControls.keyboard.iHotkey, cfgControls.keyboard.iModifier };
            gamepad = CLib::KeyCombo{ cfgControls.gamepad.iHotkey, cfgControls.gamepad.iModifier };
            kbExtraExit = CLib::Key{ cfgControls.keyboard.iExtraExit };
            gpExtraExit = CLib::Key{ cfgControls.gamepad.iExtraExit };
        }

        void Reset() noexcept
        {
            keyboard.Reset();
            gamepad.Reset();
            kbExtraExit.Reset();
            gpExtraExit.Reset();
        }

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

    MenuOpenHotkeyContext  openCtx;
    MenuCloseHotkeyContext closeCtx;

    template <class HotkeyContext>
    inline void ProcessHotkey(HotkeyContext* ctx, const RE::InputEvent* const* a_event)
    {
        ctx->Reset();
        for (auto event = *a_event; event; event = event->next) {
            if (auto button = event->AsButtonEvent()) {
                ctx->Update(button);
            }
        }
        ctx->Finalize();
    }
}

void InputManager::Process(const RE::InputEvent* const* a_event)
{
    if (Configuration::IsVersionChanged(_configVersion)) {
        std::shared_lock configLock{ Configuration::Mutex() };

        openCtx.Load();
        closeCtx.Load();

        _configVersion = Configuration::Version();

        SKSE::log::debug("InputManager: Upgrade to Configuration Version {}.", _configVersion);
    }

    if (InputBlocker::IsNotBlocked()) {
        ProcessHotkey(std::addressof(openCtx), a_event);
    } else {
        ImGui::TranslateInputEvent(a_event);
        ProcessHotkey(std::addressof(closeCtx), a_event);
    }
}

void InputManager::Cleanup() { ImGui::ClearInputEvent(); }
