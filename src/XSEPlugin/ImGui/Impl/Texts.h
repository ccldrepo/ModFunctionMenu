#pragma once

namespace ImGui::Impl
{
    class Texts
    {
    public:
        // Initialize or reload texts.
        void Load();

        std::string Title;
        std::string Section_Mod;
        std::string Section_Config;
    };
}
