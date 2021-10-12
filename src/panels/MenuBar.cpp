//
// Created by steffen on 12.10.21.
//

#include <imgui.h>
#include "MenuBar.h"

namespace vOS
{
    void MenuBar::show()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {}

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {}

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {}

                if (ImGui::MenuItem("Exit"))
                {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {}

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {}

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {}

                if (ImGui::MenuItem("Exit"))
                {}
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }
}
