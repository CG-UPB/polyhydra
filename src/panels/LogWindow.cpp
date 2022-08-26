
#include "LogWindow.h"

#include <utility>
#include "../input/Input.h"

namespace volumeshOS::Internal
{
    LogWindow::LogWindow() = default;

    void LogWindow::show(float max_x, float max_y)
    {

        // calculate max height needed
        float max_items = max_msgs < messages.size() ? (float)max_msgs : (float)messages.size();
        float min_height = ImGui::GetFrameHeight() - ImGui::GetStyle().FramePadding.y;
        float max_height = min_height * (max_msgs);

        max_height = max_height > min_height ? max_height : min_height;

        // position above the arrow_button
        auto end = ImVec2(ImGui::GetStyle().FramePadding.x, max_y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
        auto start = ImVec2(end.x, end.y - max_height);

        m_height = end.y - start.y;

        //ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().FramePadding.x, end.y - max_height));
        ImGui::SetCursorPos(ImVec2(end.x, end.y - m_height));

        if(m_visible)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
            ImGui::BeginChild("child", {m_width , m_height + ImGui::GetFrameHeight() }, false);//, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::BeginChildFrame(1, {m_width , m_height}, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
            ImGui::BeginGroup();
            ImGui::BeginTable("Messages", 3);
            for(const auto& message : messages)
            {
                show_message(message);
            }
            ImGui::EndTable();
            ImGui::EndGroup();

            if (m_adjust_scrollbar)
            {
                ImGui::SetScrollHereY(1.0f);
                m_adjust_scrollbar = false;
            }
            ImGui::EndChildFrame();

            ImGui::SetCursorPos(ImVec2(0.0f, m_height ));
            if (ImGui::ArrowButton("##arrow", m_arrow_dir))
            {
                switch_arrow();
            }


            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::EndChild();

        }
        else
        {
            ImGui::SetCursorPos(end);
            if (ImGui::ArrowButton("##arrow", m_arrow_dir))
            {
                switch_arrow();
            }
        }
    }

    void LogWindow::add_message(std::string msg, Type type)
    {
        Message message = {};
        message.msg = std::move(msg);
        message.type = type;
        messages.push_back(message);

        m_adjust_scrollbar = true;

    }

    void LogWindow::show_message(const Message& message)
    {
        // Type | Time | Message
        // -----+------+---------
        // Type | Time | Message

        ImGui::PushID(message.msg.c_str());
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() / 2.0f);
        ImGui::Text("%s", type_to_string[message.type]);
        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
        ImGui::Text("%s", message.msg.c_str());
        ImGui::PopID();
    }

    void LogWindow::switch_arrow()
    {

        m_adjust_scrollbar = true;
        if (m_arrow_dir == ImGuiDir_Up)
        {
            m_arrow_dir = ImGuiDir_Right;
            m_visible = false;
        }
        else
        {
            m_arrow_dir = ImGuiDir_Up;
            m_visible = true;
        }
    }

    Time LogWindow::get_time()
    {
        Time time = {};

        time.hours = 0;
        time.minutes = 0;
        time.seconds = 0;
        return time;
    }

    void LogWindow::hide_log_window(bool hide)
    {
        if((hide && m_visible) || (!hide && !m_visible))
        {
            switch_arrow();
        }

    }

    void LogWindow::clear_logs()
    {
        messages.clear();
    }


}
