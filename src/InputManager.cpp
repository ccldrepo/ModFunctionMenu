#include "InputManager.h"

#include "HotkeyManager.h"

void InputManager::Process(const RE::InputEvent* const* a_event) { HotkeyManager::Process(a_event); }
