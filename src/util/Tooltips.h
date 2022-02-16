#pragma once

#include "imgui.h"

#include <imgui_internal.h>

namespace vOS
{
    class Tooltips
    {
        static const int m_timer_treshold = 3;
    public:

        // Helper to display a little (?) mark which shows a tooltip when hovered.
        // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
        static void HelpMarkerWithQuestionMark(const char* desc)
        {
            ImGui::TextDisabled("(?)");
            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(desc);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }

        // Helper for Tooltip-functionality
        // Shows a tooltip if the Cursors hovers over the last Imgui-Item more than m_timer_treshold seconds
        static void ToolTipByHovering(const char* desc)
        {
            if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted(desc);
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }
    };
}