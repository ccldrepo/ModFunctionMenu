#include "Hooks.h"

#include "InputManager.h"

void ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event)
{
    if (a_event) {
        InputManager::Process(a_event);
    }

    if (!InputManager::IsNotBlocked()) {
        constexpr RE::InputEvent* const dummy[] = { nullptr };
        func(a_dispatcher, dummy);
    } else {
        func(a_dispatcher, a_event);
    }

    InputManager::HandleWantUnblock();
}
