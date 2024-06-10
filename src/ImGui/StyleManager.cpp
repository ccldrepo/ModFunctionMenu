#include "StyleManager.h"

#include <imgui.h>

#include "Configuration.h"

namespace ImGui
{
    inline float ByteToFloat(std::uint32_t a_int) { return static_cast<float>(a_int & 0xFF) / 255.0f; }

    inline ImVec4 IntToColor(std::uint32_t a_int)
    {
        return ImVec4{ ByteToFloat(a_int >> 24), ByteToFloat(a_int >> 16), ByteToFloat(a_int >> 8),
            ByteToFloat(a_int) };
    }

    void StyleManager::Init()
    {
        auto config = Configuration::GetSingleton();

        auto& style = ImGui::GetStyle();
        auto  colors = style.Colors;

        colors[ImGuiCol_Text] = IntToColor(config->styles.colors.iText);
        colors[ImGuiCol_TextDisabled] = IntToColor(config->styles.colors.iTextDisabled);
        colors[ImGuiCol_WindowBg] = IntToColor(config->styles.colors.iWindowBg);
        colors[ImGuiCol_ChildBg] = IntToColor(config->styles.colors.iChildBg);
        colors[ImGuiCol_PopupBg] = IntToColor(config->styles.colors.iPopupBg);
        colors[ImGuiCol_Border] = IntToColor(config->styles.colors.iBorder);
        colors[ImGuiCol_BorderShadow] = IntToColor(config->styles.colors.iBorderShadow);
        colors[ImGuiCol_FrameBg] = IntToColor(config->styles.colors.iFrameBg);
        colors[ImGuiCol_FrameBgHovered] = IntToColor(config->styles.colors.iFrameBgHovered);
        colors[ImGuiCol_FrameBgActive] = IntToColor(config->styles.colors.iFrameBgActive);
        colors[ImGuiCol_TitleBg] = IntToColor(config->styles.colors.iTitleBg);
        colors[ImGuiCol_TitleBgActive] = IntToColor(config->styles.colors.iTitleBgActive);
        colors[ImGuiCol_TitleBgCollapsed] = IntToColor(config->styles.colors.iTitleBgCollapsed);
        colors[ImGuiCol_MenuBarBg] = IntToColor(config->styles.colors.iMenuBarBg);
        colors[ImGuiCol_ScrollbarBg] = IntToColor(config->styles.colors.iScrollbarBg);
        colors[ImGuiCol_ScrollbarGrab] = IntToColor(config->styles.colors.iScrollbarGrab);
        colors[ImGuiCol_ScrollbarGrabHovered] = IntToColor(config->styles.colors.iScrollbarGrabHovered);
        colors[ImGuiCol_ScrollbarGrabActive] = IntToColor(config->styles.colors.iScrollbarGrabActive);
        colors[ImGuiCol_CheckMark] = IntToColor(config->styles.colors.iCheckMark);
        colors[ImGuiCol_SliderGrab] = IntToColor(config->styles.colors.iSliderGrab);
        colors[ImGuiCol_SliderGrabActive] = IntToColor(config->styles.colors.iSliderGrabActive);
        colors[ImGuiCol_Button] = IntToColor(config->styles.colors.iButton);
        colors[ImGuiCol_ButtonHovered] = IntToColor(config->styles.colors.iButtonHovered);
        colors[ImGuiCol_ButtonActive] = IntToColor(config->styles.colors.iButtonActive);
        colors[ImGuiCol_Header] = IntToColor(config->styles.colors.iHeader);
        colors[ImGuiCol_HeaderHovered] = IntToColor(config->styles.colors.iHeaderHovered);
        colors[ImGuiCol_HeaderActive] = IntToColor(config->styles.colors.iHeaderActive);
        colors[ImGuiCol_Separator] = IntToColor(config->styles.colors.iSeparator);
        colors[ImGuiCol_SeparatorHovered] = IntToColor(config->styles.colors.iSeparatorHovered);
        colors[ImGuiCol_SeparatorActive] = IntToColor(config->styles.colors.iSeparatorActive);
        colors[ImGuiCol_ResizeGrip] = IntToColor(config->styles.colors.iResizeGrip);
        colors[ImGuiCol_ResizeGripHovered] = IntToColor(config->styles.colors.iResizeGripHovered);
        colors[ImGuiCol_ResizeGripActive] = IntToColor(config->styles.colors.iResizeGripActive);
        colors[ImGuiCol_Tab] = IntToColor(config->styles.colors.iTab);
        colors[ImGuiCol_TabHovered] = IntToColor(config->styles.colors.iTabHovered);
        colors[ImGuiCol_TabActive] = IntToColor(config->styles.colors.iTabActive);
        colors[ImGuiCol_TabUnfocused] = IntToColor(config->styles.colors.iTabUnfocused);
        colors[ImGuiCol_TabUnfocusedActive] = IntToColor(config->styles.colors.iTabUnfocusedActive);
        colors[ImGuiCol_PlotLines] = IntToColor(config->styles.colors.iPlotLines);
        colors[ImGuiCol_PlotLinesHovered] = IntToColor(config->styles.colors.iPlotLinesHovered);
        colors[ImGuiCol_PlotHistogram] = IntToColor(config->styles.colors.iPlotHistogram);
        colors[ImGuiCol_PlotHistogramHovered] = IntToColor(config->styles.colors.iPlotHistogramHovered);
        colors[ImGuiCol_TableHeaderBg] = IntToColor(config->styles.colors.iTableHeaderBg);
        colors[ImGuiCol_TableBorderStrong] = IntToColor(config->styles.colors.iTableBorderStrong);
        colors[ImGuiCol_TableBorderLight] = IntToColor(config->styles.colors.iTableBorderLight);
        colors[ImGuiCol_TableRowBg] = IntToColor(config->styles.colors.iTableRowBg);
        colors[ImGuiCol_TableRowBgAlt] = IntToColor(config->styles.colors.iTableRowBgAlt);
        colors[ImGuiCol_TextSelectedBg] = IntToColor(config->styles.colors.iTextSelectedBg);
        colors[ImGuiCol_DragDropTarget] = IntToColor(config->styles.colors.iDragDropTarget);
        colors[ImGuiCol_NavHighlight] = IntToColor(config->styles.colors.iNavHighlight);
        colors[ImGuiCol_NavWindowingHighlight] = IntToColor(config->styles.colors.iNavWindowingHighlight);
        colors[ImGuiCol_NavWindowingDimBg] = IntToColor(config->styles.colors.iNavWindowingDimBg);
        colors[ImGuiCol_ModalWindowDimBg] = IntToColor(config->styles.colors.iModalWindowDimBg);
    }
}
