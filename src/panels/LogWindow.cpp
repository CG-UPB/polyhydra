
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
        if(once)
        {
            for (int i = 0; i < 2; i++)
            {
                messages.emplace_back("Test");

            }
            once = false;
        }

        static int max_msgs = 10;

        if(!ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize))// | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::End();
            return;
        }

        if(ImGui::Button("add"))
        {
            messages.emplace_back("Test");
        }
        ImGui::SameLine();
        if(ImGui::Button("clear"))
        {
            messages.clear();
        }

        ImGui::BeginChild("MessageArea", ImVec2(250, 100), false, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
        for(const auto& message : messages)
        {
            show_message(message);
        }
        ImGui::EndChild();
        ImGui::End();

    }

    void LogWindow::add_message(std::string msg)
    {
        messages.push_back(msg);
    }

    void LogWindow::show_message(const std::string& message)
    {
        ImGui::Text("%s", message.c_str());
    }
}
