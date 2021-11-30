
#include "ToolBar.h"
#include "../input/Input.h"
#include <algorithm>
#include "imgui.h"
#include <imgui_internal.h>
#include <iostream>
#include <string.h>

//TODO:Set fixed size and position in the window

namespace vOS
{

    ToolBar* ToolBar::instance = 0;

    // Singleton
    ToolBar* ToolBar::getInstance()
    {
        if (instance == 0)
        {
            instance = new ToolBar();
        }

        return instance;

    }

    ToolBar::ToolBar() {}

    // Destruktor
    ToolBar::~ToolBar()
    {
        //delete instance;
    }

    enum Selection{
        Vertex = 0,
        Edge = 1,
        Face = 2
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
    void ToolBar::show()
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
            ImGui::TextUnformatted("ToolBar");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        if(ImGui::CollapsingHeader("Filters"))
        {
            if(ImGui::BeginTable("split1", 1))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Slider:");
                //TODO: create a Slider
                static float slider_f = 0.5f;
                ImGui::SliderFloat("", &slider_f, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
                ImGui::Text("Peel:");
                //TODO: create peel-slider
                static float slider_f1 = 0.5f;
                ImGui::SliderFloat(" ", &slider_f1, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
                // TODO: Create isolation feature if we want to have
                // Therefore a picker has to work
                ImGui::Text("Start Isolation:");
                bool isolationStarted = false;
                static int clicked = 0;
                if(ImGui::Button("Isolationstart"))
                {
                    isolationStarted = true;
                    clicked++;
                }
                if (clicked & 1)
                {
                    ImGui::SameLine();
                    ImGui::Text("Thanks for clicking me!");
                }
                //...
                ImGui::Text("Finish Isolation:");
                if(ImGui::Button("Isolationend"))
                {
                    if (isolationStarted)
                    {
                        isolationStarted = false;
                        clicked++;
                    }
                }
                ImGui::EndTable();
            }
        }

        if (ImGui::CollapsingHeader("Rendering Options"))
        {
            if (ImGui::BeginTable("split", 1))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Color:");
                ImGui::ColorEdit4("", (float*)&m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                ImGui::SameLine(); HelpMarker(
                        "You can choose which color you want to use rendering the mesh");
                // Select an item type
                const char* item_names[] =
                        {
                                "Roundings", "Fissures", "Lines", "Flat Lines"
                        };
                static int item_type = 2;
                ImGui::Text("Separation:");
                ImGui::Combo("", &item_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
                //TODO: Fix the next line that it works:
                //ImGui::SliderFloat("",&m_separation_value, 0.0f, 1.0f,"ratio = %.3f");
                ImGui::SameLine();
                HelpMarker("You can choose between multiple Separation-types. This could be useful, if you want to watch inside of the mesh");
                ImGui::Text("Lighting:");
                const char* lighting_names[] =
                        {
                                "best(AO)", "fast(SSAO)", "local only", "none"
                        };
                static int lighting_type = 3;
                ImGui::Combo(" ", &lighting_type, lighting_names, IM_ARRAYSIZE(lighting_names), IM_ARRAYSIZE(lighting_names));
                //TODO:ADD All Elements we want to provide for Rendering
                //TODO:Link the buttons with the options on the Mesh

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }
} // namespace vOS
