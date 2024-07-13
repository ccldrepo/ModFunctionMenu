#include "Styles.h"

#include <imgui.h>

#include <XSEPlugin/Base/Configuration.h>

namespace ImGui::Impl
{
    namespace
    {
        inline float Norm(std::uint32_t a_int) { return static_cast<float>(a_int & 0xFF) / 255.0f; }

        inline ImVec4 IntToColor(std::uint32_t a_int)
        {
            return ImVec4{ Norm(a_int >> 24), Norm(a_int >> 16), Norm(a_int >> 8), Norm(a_int) };
        }
    }

    void Styles::Load()
    {
        // Assume the caller has already acquired the lock.
        auto& cfgStyles = Configuration::GetSingleton()->styles;

        auto& style = ImGui::GetStyle();
        auto  colors = style.Colors;

        colors[ImGuiCol_Text] = IntToColor(cfgStyles.colors.iText);
        colors[ImGuiCol_TextDisabled] = IntToColor(cfgStyles.colors.iTextDisabled);
        colors[ImGuiCol_WindowBg] = IntToColor(cfgStyles.colors.iWindowBg);
        colors[ImGuiCol_ChildBg] = IntToColor(cfgStyles.colors.iChildBg);
        colors[ImGuiCol_PopupBg] = IntToColor(cfgStyles.colors.iPopupBg);
        colors[ImGuiCol_Border] = IntToColor(cfgStyles.colors.iBorder);
        colors[ImGuiCol_BorderShadow] = IntToColor(cfgStyles.colors.iBorderShadow);
        colors[ImGuiCol_FrameBg] = IntToColor(cfgStyles.colors.iFrameBg);
        colors[ImGuiCol_FrameBgHovered] = IntToColor(cfgStyles.colors.iFrameBgHovered);
        colors[ImGuiCol_FrameBgActive] = IntToColor(cfgStyles.colors.iFrameBgActive);
        colors[ImGuiCol_TitleBg] = IntToColor(cfgStyles.colors.iTitleBg);
        colors[ImGuiCol_TitleBgActive] = IntToColor(cfgStyles.colors.iTitleBgActive);
        colors[ImGuiCol_TitleBgCollapsed] = IntToColor(cfgStyles.colors.iTitleBgCollapsed);
        colors[ImGuiCol_MenuBarBg] = IntToColor(cfgStyles.colors.iMenuBarBg);
        colors[ImGuiCol_ScrollbarBg] = IntToColor(cfgStyles.colors.iScrollbarBg);
        colors[ImGuiCol_ScrollbarGrab] = IntToColor(cfgStyles.colors.iScrollbarGrab);
        colors[ImGuiCol_ScrollbarGrabHovered] = IntToColor(cfgStyles.colors.iScrollbarGrabHovered);
        colors[ImGuiCol_ScrollbarGrabActive] = IntToColor(cfgStyles.colors.iScrollbarGrabActive);
        colors[ImGuiCol_CheckMark] = IntToColor(cfgStyles.colors.iCheckMark);
        colors[ImGuiCol_SliderGrab] = IntToColor(cfgStyles.colors.iSliderGrab);
        colors[ImGuiCol_SliderGrabActive] = IntToColor(cfgStyles.colors.iSliderGrabActive);
        colors[ImGuiCol_Button] = IntToColor(cfgStyles.colors.iButton);
        colors[ImGuiCol_ButtonHovered] = IntToColor(cfgStyles.colors.iButtonHovered);
        colors[ImGuiCol_ButtonActive] = IntToColor(cfgStyles.colors.iButtonActive);
        colors[ImGuiCol_Header] = IntToColor(cfgStyles.colors.iHeader);
        colors[ImGuiCol_HeaderHovered] = IntToColor(cfgStyles.colors.iHeaderHovered);
        colors[ImGuiCol_HeaderActive] = IntToColor(cfgStyles.colors.iHeaderActive);
        colors[ImGuiCol_Separator] = IntToColor(cfgStyles.colors.iSeparator);
        colors[ImGuiCol_SeparatorHovered] = IntToColor(cfgStyles.colors.iSeparatorHovered);
        colors[ImGuiCol_SeparatorActive] = IntToColor(cfgStyles.colors.iSeparatorActive);
        colors[ImGuiCol_ResizeGrip] = IntToColor(cfgStyles.colors.iResizeGrip);
        colors[ImGuiCol_ResizeGripHovered] = IntToColor(cfgStyles.colors.iResizeGripHovered);
        colors[ImGuiCol_ResizeGripActive] = IntToColor(cfgStyles.colors.iResizeGripActive);
        colors[ImGuiCol_TabHovered] = IntToColor(cfgStyles.colors.iTabHovered);
        colors[ImGuiCol_Tab] = IntToColor(cfgStyles.colors.iTab);
        colors[ImGuiCol_TabSelected] = IntToColor(cfgStyles.colors.iTabSelected);
        colors[ImGuiCol_TabSelectedOverline] = IntToColor(cfgStyles.colors.iTabSelectedOverline);
        colors[ImGuiCol_TabDimmed] = IntToColor(cfgStyles.colors.iTabDimmed);
        colors[ImGuiCol_TabDimmedSelected] = IntToColor(cfgStyles.colors.iTabDimmedSelected);
        colors[ImGuiCol_TabDimmedSelectedOverline] = IntToColor(cfgStyles.colors.iTabDimmedSelectedOverline);
        colors[ImGuiCol_PlotLines] = IntToColor(cfgStyles.colors.iPlotLines);
        colors[ImGuiCol_PlotLinesHovered] = IntToColor(cfgStyles.colors.iPlotLinesHovered);
        colors[ImGuiCol_PlotHistogram] = IntToColor(cfgStyles.colors.iPlotHistogram);
        colors[ImGuiCol_PlotHistogramHovered] = IntToColor(cfgStyles.colors.iPlotHistogramHovered);
        colors[ImGuiCol_TableHeaderBg] = IntToColor(cfgStyles.colors.iTableHeaderBg);
        colors[ImGuiCol_TableBorderStrong] = IntToColor(cfgStyles.colors.iTableBorderStrong);
        colors[ImGuiCol_TableBorderLight] = IntToColor(cfgStyles.colors.iTableBorderLight);
        colors[ImGuiCol_TableRowBg] = IntToColor(cfgStyles.colors.iTableRowBg);
        colors[ImGuiCol_TableRowBgAlt] = IntToColor(cfgStyles.colors.iTableRowBgAlt);
        colors[ImGuiCol_TextSelectedBg] = IntToColor(cfgStyles.colors.iTextSelectedBg);
        colors[ImGuiCol_DragDropTarget] = IntToColor(cfgStyles.colors.iDragDropTarget);
        colors[ImGuiCol_NavHighlight] = IntToColor(cfgStyles.colors.iNavHighlight);
        colors[ImGuiCol_NavWindowingHighlight] = IntToColor(cfgStyles.colors.iNavWindowingHighlight);
        colors[ImGuiCol_NavWindowingDimBg] = IntToColor(cfgStyles.colors.iNavWindowingDimBg);
        colors[ImGuiCol_ModalWindowDimBg] = IntToColor(cfgStyles.colors.iModalWindowDimBg);
    }
}
