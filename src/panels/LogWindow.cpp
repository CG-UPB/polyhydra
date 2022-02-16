
#include "LogWindow.h"
#include "../input/Input.h"
#include <algorithm>
#include "imgui.h"
#include <iostream>
#include <string.h>

namespace vOS
{

    LogWindow* LogWindow::instance = 0;

    LogWindow* LogWindow::getInstance()
    {
        if (instance == 0)
        {
            instance = new LogWindow();
        }

        return instance;

    }


    LogWindow::LogWindow()
    {
        // autoscroll is default activated
        autoScroll = true;
        clear();
    }


    LogWindow::~LogWindow()
    {
        //delete instance;
    }

    void LogWindow::clear()
    {
        //clear all
        Buf.clear();
        lineOffsets.clear();
        lineOffsets.push_back(0);
        colors.clear();
        colors.push_back(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void LogWindow::addLog(std::string fmt, int level) {
        addLog(fmt.c_str(), level);
    }


    void LogWindow::addLog(const char* fmt, int level_int)
    {
        int old_size = Buf.size();
        if (level_int == 0){
            Buf.append("information: ");
            colors.push_back(ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        } else if(level_int == 1){
            Buf.append("warning: ");
            colors.push_back(ImVec4(1.0f, 0.0f, 1.0f, 1.0f));
        } else if(level_int == 2){
            Buf.append("error: ");
            colors.push_back(ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        } else if(level_int == 3){
            Buf.append("critical: ");
            colors.push_back(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        } else {
            std::cout << "undefined log type";
        }

        Buf.append(fmt);
        Buf.append("\n");
        
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                lineOffsets.push_back(old_size + 1);
    }


    void LogWindow::show()
    {
        if (!ImGui::Begin("Log-ImguiRenderer"))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &autoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear1 = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear1)
            clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char* buf = Buf.begin();
        const char* buf_end = Buf.end();
        
        // implements a filter function looking for specific words 
        if (filter.IsActive())
        {
            for (int line_no = 0; line_no < lineOffsets.Size; line_no++)
            {
                const char* line_start = buf + lineOffsets[line_no];
                const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buf + lineOffsets[line_no + 1] - 1) : buf_end;
                if (filter.PassFilter(line_start, line_end))
                    // option to get colorful text -> not really working yet
                    //ImGui::TextColored(colors.at(line_no),line_start,line_end);
                    // option may be used to print white text
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // using the clipper to only process lines that are within the visible area.
            ImGuiListClipper clipper;
            clipper.Begin(lineOffsets.Size);
            while (clipper.Step())
            {
                int colLine = 0;
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + lineOffsets[line_no];
                    const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buf + lineOffsets[line_no + 1] - 1) : buf_end;
                    // option to get colorful text -> not really working yet
                    //ImGui::TextColored(colors.at(colLine++),line_start,line_end);
                    // option to get white text
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();
        ImGui::End();
    }
} // namespace vOS
