#pragma once

#include <PCH.h>

#include "../Core.h"
#include "../Util/Singleton.h"

class Menu final : public Singleton<Menu>
{
    friend class Singleton<Menu>;

public:
    bool IsOpen() const { return isOpen; }

    void Open();
    void Close();

    void Draw();

private:
    Menu() = default;

    void OnClickParentEntry(MFM_Tree* a_tree);
    void OnClickEntry(MFM_Tree* a_tree, const MFM_Node* a_node);

    std::atomic<bool> isOpen{ false };
};
