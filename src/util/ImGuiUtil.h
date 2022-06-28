#pragma once

#include "vospch.h"

#include "UIUtil.h"

namespace volumeshOS::Internal
{
    class ImGuiUtil
    {
    public:

        static void icon(const std::string& icon, float size = ImGui::GetFontSize(), bool use_text_color = false)
        {
            auto& icon_ref = *UIUtil::get_icon(icon);
            float aspect_ratio = (float) icon_ref.get_width() / (float) icon_ref.get_height();
            float width = size * aspect_ratio;
            float height = size;
            float y_pos = ImGui::GetCursorPosY() + ImGui::GetFrameHeight() / 2.0f - height / 2.0f;
            ImGui::BeginGroup();
            ImGui::SetCursorPosY(y_pos);
            if (use_text_color)
            {
                ImVec4 text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
                ImGui::Image(
                        reinterpret_cast<ImTextureID>(icon_ref.get_id()),
                        {width, height},
                        {0.0f, 0.0f},
                        {1.0f, 1.0f},
                        text_color
                );
            }
            else
            {
                ImGui::Image(reinterpret_cast<ImTextureID>(icon_ref.get_id()), {width, height});
            }
            ImGui::EndGroup();
        }

        static bool icon_button(const std::string& icon, float size = ImGui::GetFontSize(), bool use_text_color = true)
        {
            auto& icon_ref = *UIUtil::get_icon(icon);
            float aspect_ratio = (float) icon_ref.get_width() / (float) icon_ref.get_height();
            float width = size * aspect_ratio;
            float height = size;
            ImVec4 text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);
            if (use_text_color)
            {
                return ImGui::ImageButton(
                        reinterpret_cast<ImTextureID>(icon_ref.get_id()),
                        {width, height},
                        {0.0f, 0.0f},
                        {1.0f, 1.0f},
                        -1,
                        {0.0f, 0.0f, 0.0f, 0.0f},
                        text_color
                );
            }
            return ImGui::ImageButton(
                    reinterpret_cast<ImTextureID>(icon_ref.get_id()),
                    {width, height}
            );
        }

        static void add_padding_y(float multiplier = 1.0f)
        {
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y * multiplier);
        }

        static void push_bold_font()
        {
            ImGui::PushFont(UIUtil::get_bold_font());
        }
    };
}