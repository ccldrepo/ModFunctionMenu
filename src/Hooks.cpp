#include "Hooks.h"

#include "HotkeyManager.h"

void ProcessInputQueueHook::thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event)
{
    if (a_event) {
        // Prioritize our input event.
        HotkeyManager::Process(a_event);
    }

    if (HotkeyManager::shouldBlockInput) {
        // Block regular input event processing.
        constexpr RE::InputEvent* const dummy[] = { nullptr };
        return func(a_dispatcher, dummy);
    } else {
        return func(a_dispatcher, a_event);
    }
}
