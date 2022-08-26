
#include "LogWindow.h"

#include <utility>
#include "../input/Input.h"
#include "../util/ImGuiUtil.h"

namespace volumeshOS::Internal
{
    LogWindow::LogWindow() = default;
    float LogWindow::min_height = 24.0f;

    void LogWindow::show(float max_x, float max_y)
    {
        m_width = ImGui::GetContentRegionAvailWidth() / 3.0f;
        float max_height = ImGui::GetWindowHeight() / 3.0f - (float)((int)ImGui::GetWindowHeight() % (int)min_height);

        // calculate max height needed
        //float max_height = min_height * (max_msgs) + ImGui::GetStyle().ScrollbarSize;

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

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 0.0f));
            ImGui::BeginChildFrame(1, {m_width , m_height}, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoScrollbar);
            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ImVec4(0.1f, 0.1f, 0.1f, 0.1f));
            ImGui::BeginTable("Messages", 4, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollX);

            for(const auto& message : messages)
            {
                show_message(message);
            }

            if (m_adjust_scrollbar)
            {
                ImGui::SetScrollHereY(1.0f);
                m_adjust_scrollbar = false;
            }

            ImGui::EndTable();
            ImGui::EndGroup();

            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopStyleColor();
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
        message.time = time(nullptr);
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
        ImGui::Dummy(ImVec2(0.0f, min_height));
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
        //ImGui::SetNextItemWidth(ImGui::CalcTextSize(types[(int)message.type]).x);
        switch (message.type)
        {
            case Info:
                ImGuiUtil::icon("info.png", ImGui::GetFontSize(), true);
                break;
            case Warning:
                ImGuiUtil::icon("info.png", ImGui::GetFontSize(), true);
                break;
            case Error:
                ImGuiUtil::icon("info.png", ImGui::GetFontSize(), true);
                break;

        }

        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(types[(int)message.type]).x);
        ImGui::Text("%s", types[(int)message.type]);

        ImGui::TableSetColumnIndex(2);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
        char buffer[9];
        strftime(buffer, 9, "%H:%M:%S", localtime(&message.time));
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(buffer).x);
        ImGui::Text("[%s]", buffer);

        ImGui::TableSetColumnIndex(3);
        ImGui::SetNextItemWidth(ImGui::GetContentRegionMax().x);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + min_height / 2.0f));
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
