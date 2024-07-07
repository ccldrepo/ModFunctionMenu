#pragma once

#include <XSEPlugin/Util/Singleton.h>

class StringTable final : public Singleton<StringTable>
{
    friend class Singleton<StringTable>;

public:
    // Initialize or reload string table.
    void Init();

    std::string Title;
    std::string Section_Mod;
    std::string Section_Config;

private:
    StringTable() { Init(); }

    ~StringTable() = default;
};
