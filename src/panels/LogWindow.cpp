
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
        autoScroll = true;
        clear();
    }


    LogWindow::~LogWindow()
    {
        delete instance;
    }

    void LogWindow::clear()
    {
        Buf.clear();
        lineOffsets.clear();
        lineOffsets.push_back(0);
    }


    //TODO: Add new Line, if a Log-Message is to big for the Screen
    void LogWindow::addLog(const char* fmt, int level_int)
    {
        int old_size = Buf.size();
        if (level_int == 0){
            Buf.append("information: ");
        } else if(level_int == 1){
            Buf.append("warning: ");
        } else if(level_int == 2){
            Buf.append("errror: ");
        } else if(level_int == 3){
            Buf.append("critical: ");
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
        if (!ImGui::Begin("Log-Window"))
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
        if (filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of
            // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < lineOffsets.Size; line_no++)
            {
                const char* line_start = buf + lineOffsets[line_no];
                const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buf + lineOffsets[line_no + 1] - 1) : buf_end;
                if (filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
            // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
            // within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
            // on your side is recommended. Using ImGuiListClipper requires
            // - A) random access into your data
            // - B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display
            // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
            // it possible (and would be recommended if you want to search through tens of thousands of entries).
            ImGuiListClipper clipper;
            clipper.Begin(lineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char* line_start = buf + lineOffsets[line_no];
                    const char* line_end = (line_no + 1 < lineOffsets.Size) ? (buf + lineOffsets[line_no + 1] - 1) : buf_end;
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
