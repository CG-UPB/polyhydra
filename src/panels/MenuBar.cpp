
#include "MenuBar.h"

#include <imgui.h>

namespace vOS
{
    void MenuBar::show()
    {
        // this should of course be changed to serve a function, for now this is just a placeholder
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
