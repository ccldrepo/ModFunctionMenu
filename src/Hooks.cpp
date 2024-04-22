#include "Hooks.h"

#include "InputManager.h"

void ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event)
{
    if (a_event) {
        InputManager::Process(a_event);
    }

    constexpr RE::InputEvent* const dummy[] = { nullptr };
    func(a_dispatcher, InputManager::IsNotBlocked() ? a_event : dummy);

    InputManager::HandleWantUnblock();
}
