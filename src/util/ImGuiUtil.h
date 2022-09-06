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

        static void add_background_rect(int num_items, uint32_t color = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Tab)))
        {
            ImVec2 p = ImGui::GetCursorScreenPos();
            p.x = p.x -  ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().FramePadding.x;
            p.y = p.y + ImGui::GetStyle().ItemSpacing.y + 1.0f;
            ImVec2 c = ImGui::GetContentRegionMax();
            c.x = c.x + ImGui::GetStyle().FramePadding.x;
            c.y = c.y - ImGui::GetStyle().WindowPadding.y;
            auto pos = ImGui::GetCursorPos();
            ImGui::Dummy({1, (ImGui::GetFrameHeight() + ImGui::GetStyle().CellPadding.y * 2) * (float) num_items});
            float border = 2.0f;
            auto border_color = ImGui::GetColorU32(ImGui::GetStyleColorVec4(ImGuiCol_Border));
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(c.x, ImGui::GetCursorScreenPos().y + ImGui::GetStyle().WindowPadding.y - 5 + border), border_color, ImGui::GetStyle().FrameRounding);
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(c.x, ImGui::GetCursorScreenPos().y + ImGui::GetStyle().WindowPadding.y - 5), color, ImGui::GetStyle().FrameRounding);
            ImGui::SetCursorPos({pos.x + ImGui::GetStyle().WindowPadding.x * 0.5f, pos.y + ImGui::GetStyle().WindowPadding.y * 0.5f});
        }

        static bool begin_menu_with_background(const std::string& name, int item_count, ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame)
        {
            ImGuiUtil::add_background_rect(item_count);
            ImGui::BeginGroup();
            return ImGui::BeginTable(name.c_str(), 2, flags);
        }

        static bool begin_menu(const std::string& name)
        {
            ImGui::BeginGroup();
            return ImGui::BeginTable(name.c_str(), 2);
        }

        static void end_menu()
        {
            ImGui::EndTable();
            ImGui::EndGroup();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y * 0.5f);
        }

        template<typename VoidFunc>
        static void menu_item_filled(const std::string& name, const VoidFunc& item)
        {
            ImGui::PushID(name.c_str());
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
            item();
            ImGui::PopID();
        }

        template<typename VoidFunc>
        static void menu_item(const std::string& name,float width, const VoidFunc& item)
        {
            ImGui::PushID(name.c_str());
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() - width);
            item();
            ImGui::PopID();
        }
    };
}