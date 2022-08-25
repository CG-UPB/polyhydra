
#include "LogWindow.h"
#include "../input/Input.h"

namespace volumeshOS::Internal
{
    LogWindow::LogWindow() = default;

    void LogWindow::show(float max_x, float max_y)
    {

        // calculate max height needed
        float max_items = max_msgs < messages.size() ? (float)max_msgs : (float)messages.size();
        float max_height = ImGui::GetFrameHeightWithSpacing() * (max_items) +  2 * ImGui::GetStyle().WindowPadding.y;


        // position above the arrow_button
        auto end = ImVec2(ImGui::GetStyle().FramePadding.x, max_y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().WindowPadding.y);
        auto start = ImVec2(end.x, end.y - max_height);

        m_height = end.y - start.y;

        //ImGui::SetCursorPos(ImVec2(ImGui::GetStyle().FramePadding.x, end.y - max_height));

        static bool once = true;
        if (once)
        {
            for (int i = 0; i < 34; i++)
            {
                add_message("Test " + std::to_string(i));
            }
            once = false;
        }

        ImGui::SetCursorPos(ImVec2(end.x, end.y - m_height));

        if(m_visible)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
            ImGui::BeginChild("child", {m_width , m_height + ImGui::GetFrameHeight()}, false);//, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::BeginChildFrame(1, {m_width , m_height}, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
            ImGui::BeginGroup();
            for(const auto& message : messages)
            {
                show_message(message);
            }
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


//        ImGui::SameLine(0.0f, 0.0f);
//        if (ImGui::Button("", ImVec2(m_width - ImGui::GetCursorPosX(), 0.0f)))
//        {
//            switch_arrow();
//        }

    }

    void LogWindow::add_message(const std::string& msg)
    {
        messages.push_back(msg);
        m_adjust_scrollbar = true;
    }

    void LogWindow::show_message(const std::string& message)
    {
        ImGui::Text("%s", message.c_str());
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


}
