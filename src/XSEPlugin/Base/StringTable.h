#pragma once

#include <XSEPlugin/Util/Singleton.h>

class StringTable final : public Singleton<StringTable>
{
    friend class Singleton<StringTable>;

public:
    // Reload string table if necessary.
    void CheckReload();

    std::string Title;
    std::string Section_Mod;
    std::string Section_Config;

private:
    StringTable() = default;

    ~StringTable() = default;
};
