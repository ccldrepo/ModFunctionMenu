#pragma once

namespace ImGui::Impl
{
    class Texts
    {
    public:
        /// Fetch and apply the latest texts.
        ///
        /// Assume caller has already acquired shared lock of translation before calling.
        void Load();

        std::string Title;
        std::string Section_Mod;
        std::string Section_Config;
    };
}
