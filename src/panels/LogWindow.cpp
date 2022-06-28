
#include "LogWindow.h"
#include "../input/Input.h"

namespace volumeshOS::Internal
{
    std::shared_ptr<LogWindow> LogWindow::s_instance;

    std::shared_ptr<LogWindow> LogWindow::getInstance()
    {
        if (s_instance == nullptr)
        {
            s_instance = std::shared_ptr<LogWindow>(new LogWindow());
        }
        return s_instance;
    }


    LogWindow::LogWindow() : m_auto_scroll(true)
    {
        clear();
    }

    void LogWindow::clear()
    {
        //clear all
        m_buffer.clear();
        m_line_offsets.clear();
        m_line_offsets.push_back(0);
        m_colors.clear();
        m_colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void LogWindow::addLog(const std::string& fmt, int level)
    {
        addLog(fmt.c_str(), level);
    }

    // following methods are similar to https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp [SECTION] Example App: Debug Log
    // sends messages to the log console
    void LogWindow::addLog(const char* fmt, int level_int)
    {
        int old_size = m_buffer.size();
        if (level_int == 0)
        {
            m_buffer.append("information: ");
            m_colors.emplace_back(0.0f, 1.0f, 1.0f, 1.0f);
        }
        else if (level_int == 1)
        {
            m_buffer.append("warning: ");
            m_colors.emplace_back(1.0f, 0.0f, 1.0f, 1.0f);
        }
        else if (level_int == 2)
        {
            m_buffer.append("error: ");
            m_colors.emplace_back(1.0f, 1.0f, 0.0f, 1.0f);
        }
        else if (level_int == 3)
        {
            m_buffer.append("critical: ");
            m_colors.emplace_back(1.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            std::cout << "undefined log type";
        }

        m_buffer.append(fmt);
        m_buffer.append("\n");

        for (int new_size = m_buffer.size(); old_size < new_size; old_size++)
            if (m_buffer[old_size] == '\n')
                m_line_offsets.push_back(old_size + 1);
    }


    void LogWindow::show()
    {
        if (!ImGui::Begin("Log"))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &m_auto_scroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear1 = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");

        m_filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear1)
            clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = m_buffer.begin();
        const char* buf_end = m_buffer.end();

        // implements a filter function looking for specific words 
        if (m_filter.IsActive())
        {
            for (int line_no = 0; line_no < m_line_offsets.Size; line_no++)
            {
                const char* line_start = buf + m_line_offsets[line_no];
                const char* line_end = (line_no + 1 < m_line_offsets.Size) ? (buf + m_line_offsets[line_no + 1] - 1)
                                                                           : buf_end;
                if (m_filter.PassFilter(line_start, line_end))
                    // option to get_rgb colorful text -> not really working yet
                    //ImGui::TextColored(colors.at(line_no),line_start,line_end);
                    // option may be used to print white text
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // using the clipper to only process lines that are within the visible area.
            ImGuiListClipper clipper;
            clipper.Begin(m_line_offsets.Size);
            while (clipper.Step())
            {
                int colLine = 0;
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + m_line_offsets[line_no];
                    const char* line_end = (line_no + 1 < m_line_offsets.Size) ? (buf + m_line_offsets[line_no + 1] - 1)
                                                                               : buf_end;
                    // option to get_rgb colorful text -> not really working yet
                    //ImGui::TextColored(colors.at(colLine++),line_start,line_end);
                    // option to get_rgb white text
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
}
