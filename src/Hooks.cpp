#include "Hooks.h"

#include "HotkeyManager.h"

namespace
{
    struct ProcessInputQueue
    {
        static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event)
        {
            if (!a_event) {
                return func(a_dispatcher, a_event);
            }

            // Prioritize our input event.
            HotkeyManager::Process(a_event);

            if (HotkeyManager::shouldBlockInput) {
                // Block regular input event processing.
                constexpr RE::InputEvent* const dummy[] = { nullptr };
                return func(a_dispatcher, dummy);
            }
            return func(a_dispatcher, a_event);
        }

        static inline REL::Relocation<decltype(thunk)> func;
    };
}

void Hooks::Install()
{
    REL::Relocation<std::uintptr_t> target(REL::RelocationID(67315, 68617), 0x7B);
    SKSE::stl::write_thunk_call<ProcessInputQueue>(target);
    SKSE::log::info("Hooked 0x{:016X} call.", target.address());
}
