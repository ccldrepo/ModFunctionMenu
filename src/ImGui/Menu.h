#pragma once

#include <PCH.h>

#include "../Util/Singleton.h"

class Menu final : public Singleton<Menu>
{
    friend class Singleton<Menu>;

public:
    bool IsOpen() const { return isOpen; }

    void Open();
    void Close();
    void Toggle();

    void Draw();

private:
    Menu() = default;

    bool isOpen{ false };
};
