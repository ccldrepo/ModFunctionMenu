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
    Menu() { ResetCurrentSection(); };

    MFM_Tree* CurrentSection() noexcept { return currentSection; }
    void      CurrentSection(MFM_Tree& a_tree) { CurrentSection(std::addressof(a_tree)); }
    void      CurrentSection(MFM_Tree* a_tree) { currentSection = a_tree; }

    void ResetCurrentSection() { CurrentSection(modTree); }

    void DrawExplorer();
    void DrawMessageBox();

    void OnClickParentEntry(MFM_Tree* a_tree);
    void OnClickEntry(MFM_Tree* a_tree, const MFM_Node* a_node);

    void InvokeFunction(const MFM_Function& a_func);

    std::atomic<bool> isOpen{ false };

    MFM_Tree  modTree{ MFM_Path::mod };
    MFM_Tree  configTree{ MFM_Path::config };
    MFM_Tree* currentSection;

    std::vector<char> _msg;
};
