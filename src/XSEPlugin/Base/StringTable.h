#pragma once

class StringTable
{
public:
    [[nodiscard]] static StringTable* GetSingleton() { return std::addressof(_singleton); }

    StringTable(const StringTable&) = delete;
    StringTable(StringTable&&) = delete;
    StringTable& operator=(const StringTable&) = delete;
    StringTable& operator=(StringTable&&) = delete;

    // Reload string table if necessary.
    void CheckReload();

    std::string Title;
    std::string Section_Mod;
    std::string Section_Config;

private:
    StringTable() = default;

    ~StringTable() = default;

    static StringTable _singleton;

    std::uint32_t _transVersion{ 0 };
};
