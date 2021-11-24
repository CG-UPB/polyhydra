
#include "Test.h"
#include "../input/Input.h"
#include <algorithm>
#include "imgui.h"
#include <imgui_internal.h>
#include <iostream>
#include <string.h>

namespace vOS
{

    Test* Test::instance = 0;

    // Singleton
    Test* Test::getInstance()
    {
        if (instance == 0)
        {
            instance = new Test();
        }

        return instance;

    }

    Test::Test() {}

    // Destruktor
    Test::~Test()
    {
        //delete instance;
    }

    enum Selection{
        Vertex,
        Edge,
        Face
    };

    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // show log window and corresponding buttons
    void Test::show()
    {
        if(!ImGui::Begin("Toolbox"))
        {
            ImGui::End();
            return;
        }


        if(ImGui::Button("Snapshot"))
        {
            //TODO:Start file dialog to save a snapshot
            //TODO:Make clear how to get image of the actual mesh object (with transparent background)
        }
        ImGui::SameLine();
        HelpMarker("With this Button you can use the Snapshot-function. It will open a file dialog, where you can "
                   "choose a file in which you want to save your image of the actual Mesh");

        if (ImGui::BeginPopup("Selection")) {
            //ImGui::Checkbox("Vertex-Selection", &m_vertex_selection);
            if (ImGui::RadioButton("Vertex-Selection", m_current_selection_mode == Vertex))
                m_current_selection_mode = Vertex;

            if (ImGui::RadioButton("Edge-Selection", m_current_selection_mode == Edge))
                m_current_selection_mode = Edge;

            if (ImGui::RadioButton("Face-Selection", m_current_selection_mode == Face))
                m_current_selection_mode = Face;
            ImGui::EndPopup();
        }
        if (ImGui::Button("Selection"))
            ImGui::OpenPopup("Selection");
        if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("Test");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        if (ImGui::CollapsingHeader("Rendering Options"))
        {
            if (ImGui::BeginTable("split", 3))
            {
                ImGui::ColorEdit4("Color", (float*)&m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine(); HelpMarker(
                        "You can choose which color you want to use rendering the mesh");
                // Select an item type
                const char* item_names[] =
                        {
                                "Roundings", "Fissures", "Lines", "Flat Lines"
                        };
                static int item_type = 4;
                static float col4f[4] = { 1.0f, 0.5, 0.0f, 1.0f };
                ImGui::Combo("Separation", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
                ImGui::SameLine();
                //ImGui::SliderFloat(&col4f[0], 0.0f, 1.0f)
                //ImGui::SameLine();
                HelpMarker("You can choose between multiple Separation-types. This could be useful, if you want to watch inside of the mesh");

                //TODO:ADD All Elements we want to provide for Rendering
                //TODO:Link the buttons with the options on the Mesh

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }
} // namespace vOS
