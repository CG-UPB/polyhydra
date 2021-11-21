//
// Created by Max König on 17.11.21.
//
#include "ToolBox.h"

#include <imgui.h>

namespace vOS
{
    void ToolBox::show()
    {
        if(!ImGui::Begin("Toolbox"))
        {
            ImGui::End();
            return;
        }

        if (ImGui::BeginPopup("Selection"))
        {
            ImGui::Checkbox("Vertex-Selection", &m_vertex_selection);
            ImGui::EndPopup();
        }


        ImGui::End();
    }
}