#pragma once

namespace ImGui::Impl
{
    class Styles
    {
    public:
        /// Fetch and apply the latest styles.
        ///
        /// @note
        ///   Assume caller has already acquired shared lock of
        ///   configuration before calling.
        void Load();
    };
}
