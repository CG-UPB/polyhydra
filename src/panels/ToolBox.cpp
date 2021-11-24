//
// Created by Max König on 17.11.21.
//
#include "ToolBox.h"

#include <imgui.h>

namespace vOS
{
    enum Selection{
        Vertex,
        Edge,
        Face
    };

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

        if (ImGui::RadioButton("Vertex-Selection", m_current_selection_mode == Vertex))
            m_current_selection_mode = Vertex;

        if (ImGui::RadioButton("Edge-Selection", m_current_selection_mode == Edge))
            m_current_selection_mode = Edge;

        if (ImGui::RadioButton("Face-Selection", m_current_selection_mode == Face))
            m_current_selection_mode = Face;

        ImGui::End();
    }
}