#pragma once

#include <PCH.h>

#include "Menu.h"

class HotkeyManager
{
public:
    static void ProcessMenuOpen(const RE::InputEvent* const* a_event, Menu* a_menu);

    static void ProcessMenuClose(const RE::InputEvent* const* a_event, Menu* a_menu);
};
