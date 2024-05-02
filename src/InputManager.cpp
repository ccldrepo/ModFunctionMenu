#include "InputManager.h"

#include "HotkeyManager.h"
#include "ImGui/Input.h"

void InputManager::Process(const RE::InputEvent* const* a_event)
{
    auto menu = Menu::GetSingleton();
    if (!menu->IsOpen()) {
        HotkeyManager::ProcessMenuOpen(a_event, menu);
    } else {
        ImGui::TranslateInputEvent(a_event);
        HotkeyManager::ProcessMenuClose(a_event, menu);
    }
}
