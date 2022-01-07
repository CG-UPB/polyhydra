#include "ToolBar.h"
#include "../input/Input.h"
#include <algorithm>
#include "imgui.h"
#include <imgui_internal.h>
#include <iostream>
#include <string.h>
#include "MeshView.h"
#include "ImGuiFileDialog.h"

#include <iostream>
#include <fstream>

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



    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
    static void HelpMarkerWithQuestionMark(const char* desc)
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
            m_open_file = true;
        }
        if(m_open_file)
        {
            std::string path;
            if (Window::ShowFileDialog(path,".bmp",1))
            {
                GlobalViewerSettings::getInstance()->m_set_take_snapshot(true);
                GlobalViewerSettings::getInstance()->m_set_actual_snapshot_filename(path);
                LogWindow::getInstance()->addLog("hier" + path);
                m_open_file = false;
            }
            

        }
        ImGui::SameLine();
        HelpMarkerWithQuestionMark("With this Button you can use the Snapshot-function. It will open a file dialog, where you can "
                   "choose a file in which you want to save your image of the actual Mesh");

        if (ImGui::BeginPopup("Selection")) {
            //ImGui::Checkbox("Vertex-Selection", &m_vertex_selection);
            if (ImGui::RadioButton("Vertex-Selection", m_current_selection_mode == Vertex))
            {
                m_current_selection_mode = Vertex;
                GlobalViewerSettings::getInstance()->m_set_current_selection_mode(Vertex);
            }
            ImGui::SameLine(); HelpMarkerWithQuestionMark("This button will select the nearest Vertex of your pick");
            if (ImGui::RadioButton("Edge-Selection", m_current_selection_mode == Edge))
            {
                m_current_selection_mode = Edge;
                GlobalViewerSettings::getInstance()->m_set_current_selection_mode(Edge);
            }
            ImGui::SameLine(); HelpMarkerWithQuestionMark("This button will select the nearest Edge of your pick");
            if (ImGui::RadioButton("Face-Selection", m_current_selection_mode == Face))
            {
                m_current_selection_mode = Face;
                GlobalViewerSettings::getInstance()->m_set_current_selection_mode(Face);
            }
            ImGui::SameLine(); HelpMarkerWithQuestionMark("This button will select the nearest Face of your pick");
            ImGui::EndPopup();
        }
        if (ImGui::Button("Selection"))
            ImGui::OpenPopup("Selection");
        if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("By pushing this button diverse options for Selection of elements where shown. "
                                   "You can decide which one you want to use");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        if(ImGui::CollapsingHeader("Filters"))
        {
            if(ImGui::BeginTable("split1", 1))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Slicer:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("This slider will slice through the mesh to show an "
                                                              "inview of the mesh");
                ImGui::SliderInt("", &m_slider_slicer, 0, 10);
                GlobalViewerSettings::getInstance()->m_set_current_mesh_slice_level(m_slider_slicer);
                ImGui::Text("Peel:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("This slider will peel the mesh like an onion");
                ImGui::SliderInt(" ", &m_slider_peel, 0, 10);
                GlobalViewerSettings::getInstance()->m_set_current_mesh_peel_level(m_slider_peel);
                if (ImGui::SliderFloat("Cell Size:", &m_cell_size, 0.0f, 1.0f))
                {
                    GlobalViewerSettings::getInstance()->m_set_current_cell_size(m_cell_size);
                }
                // Therefore a picker has to work
                ImGui::Text("Start Isolation:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("With this button you can start a selection of a "
                                                              "variable number of vertices, edges or faces, which you "
                                                              "want to visualize for their own. To stop the selection "
                                                              "you have to press the button below");
                static int clicked = 0;
                if(ImGui::Button("Isolationstart"))
                {
                    if(!m_isolation_started)
                    {
                        m_isolation_started = true;
                        GlobalViewerSettings::getInstance()->m_set_current_isolation_state(m_isolation_started);
                        clicked++;
                    }
                }
                if (clicked & 1)
                {
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f),"Isolation-Selection is Active");
                }
                //...
                ImGui::Text("Finish Isolation:");
                if(ImGui::Button("Isolationend"))
                {
                    if (m_isolation_started)
                    {
                        m_isolation_started = false;
                        GlobalViewerSettings::getInstance()->m_set_current_isolation_state(m_isolation_started);
                        clicked++;
                    }
                }
                ImGui::EndTable();
            }
        }
        if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("By pushing this button diverse options for Filtering the mesh where shown. "
                                   "There are multiple options to get a better view of the mesh");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        if (ImGui::CollapsingHeader("Rendering Options"))
        {
            if (ImGui::BeginTable("split", 1))
            {
                ImGui::TableNextColumn();
                ImGui::Text("Color:");
                ImGui::Checkbox("", &m_color_activated);
                ImGui::SameLine();
                ImGui::ColorEdit4("", m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                GlobalViewerSettings::getInstance()->m_set_current_mesh_rendering_color(m_color_activated, m_color[0],m_color[1],m_color[2],m_color[3]);
                ImGui::SameLine(); HelpMarkerWithQuestionMark(
                        "You can choose which color you want to use rendering the mesh");
                
                // Select an item type
                const char* rendering_mode_names[] =
                        {
                                "WireFrame", "Phong"
                        };
                ImGui::Text("Rendering Mode:");
                ImGui::Combo("  ", &m_rendering_mode, rendering_mode_names, IM_ARRAYSIZE(rendering_mode_names), IM_ARRAYSIZE(rendering_mode_names));
                GlobalViewerSettings::getInstance()->m_set_current_rendering_mode(m_rendering_mode);
                ImGui::SameLine();
                HelpMarkerWithQuestionMark("You can choose between multiple rendering modes for the mesh");
                
                
                // Select an item type
                const char* item_names[] =
                        {
                                "Roundings", "Fissures", "Lines", "Flat Lines"
                        };
                ImGui::Text("Separation:");
                ImGui::Combo("   ", &m_separation_type, item_names, IM_ARRAYSIZE(item_names), IM_ARRAYSIZE(item_names));
                GlobalViewerSettings::getInstance()->m_set_current_separation_type(m_separation_type);
                //TODO: Fix the next line that it works:
                //ImGui::SliderFloat("",&m_separation_value, 0.0f, 1.0f,"ratio = %.3f");
                ImGui::SameLine();
                HelpMarkerWithQuestionMark("You can choose between multiple Separation-types. This could be useful, if you want to watch inside of the mesh");
                ImGui::Text("Lighting:");
                const char* lighting_names[] =
                        {
                                "best(AO)", "fast(SSAO)", "local only", "none"
                        };
                ImGui::Combo(" ", &m_lighting_type, lighting_names, IM_ARRAYSIZE(lighting_names), IM_ARRAYSIZE(lighting_names));
                GlobalViewerSettings::getInstance()->m_set_current_lighting_type(m_lighting_type);
                ImGui::SameLine(); HelpMarkerWithQuestionMark("You can decide which lighthing you want to use");
                //TODO:ADD All Elements we want to provide for Rendering
                //TODO:Link the buttons with the options on the Mesh

                ImGui::EndTable();
            }
        }
        if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("By pushing this button diverse options for Rendering the mesh. "
                                   "There are multiple options for different rendering of the mesh");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
        ImGui::End();
    }
} // namespace vOS
