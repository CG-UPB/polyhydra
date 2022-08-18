
#include "LogWindow.h"
#include "../input/Input.h"

namespace volumeshOS::Internal
{
    void LogWindow::show()
    {
        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 300, main_viewport->Size.y), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_FirstUseEver);

        static bool once = true;
        if (once)
        {
            for (int i = 0; i < 2; i++)
            {
                add_message("Test");
            }
            once = false;
        }


        static int max_msgs = 10;

        if (!ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoBackground |
                                          ImGuiWindowFlags_AlwaysAutoResize))// | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.0f);

        m_focused = false;
        if(ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows) && !m_focused)
        {
            m_focused = true;
            m_height = 200.0f;
        }
        else
        {

            m_height = 100.0f;
        }

        if (ImGui::Button("add"))
        {
             add_message("Test");
        }
        ImGui::SameLine();
        if (ImGui::Button("clear"))
        {
            messages.clear();
            messages.resize(0);
        }

        if(m_visible)
        {
            ImGui::BeginChild("MessageArea", ImVec2(m_width, m_height), true,
                              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);

            for(const auto& message : messages)
            {
                show_message(message);
            }

            if (m_adjust_scrollbar)
            {
                ImGui::SetScrollHereY(1.0f);
                m_adjust_scrollbar = false;
            }

            ImGui::EndChild();
        }
        auto y = ImGui::GetCursorPosY();
        if (ImGui::ArrowButton("##arrow", m_arrow_dir))
        {
            switch_arrow();
        }
        y = y - ImGui::GetCursorPosY();
        ImGui::SameLine(0.0f, 0.0f);
        if (ImGui::Button("", ImVec2(m_width - ImGui::GetCursorPosX(), 0.0f)))
        {
            switch_arrow();
        }


        ImGui::PopStyleVar();
        ImGui::End();

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
        if (m_arrow_dir == ImGuiDir_Up)
        {
            m_arrow_dir = ImGuiDir_Right;
            m_visible = false;
        }
        else
        {
            m_arrow_dir = ImGuiDir_Up;
            m_visible = true;
            m_adjust_scrollbar = true;
        }
    }
}
