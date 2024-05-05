#pragma once

#include <PCH.h>

#include "CLib/Hook.h"

struct ProcessInputQueueHook : CLib::CallHook<ProcessInputQueueHook>
{
    static void thunk(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent* const* a_event);

    static inline REL::Relocation<decltype(thunk)> func;

    static constexpr std::string_view name{ "ProcessInputQueue"sv };

    static constexpr REL::VariantID     id{ 67315, 68617, 0xC519E0 };
    static constexpr REL::VariantOffset offset{ 0x7B, 0x7B, 0x81 };
};
