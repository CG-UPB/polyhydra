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

    ToolBar::ToolBar() {
    }

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

//        m_active_mesh = Window::instance().get_mesh_active();
//        ImGui::InputInt("Mesh in Focus", &m_active_mesh);
//        if (m_active_mesh < 0) m_active_mesh = 0;
//        if (m_active_mesh >= GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes()) m_active_mesh = GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes() - 1;
//        GlobalViewerSettings::getInstance()->m_set_current_active_mesh(m_active_mesh);
//        Window::instance().set_mesh_active(m_active_mesh);
        //GlobalViewerSettings::getInstance()->m_set_current_new_active_mesh(true);

        if(ImGui::Button("Snapshot"))
        {
            Window::instance().m_file_dialog->open(".png,.bmp", 1);
        }

        if(Window::instance().m_file_dialog->file_dialogue_open(1))
        {
            std::string path = Window::instance().m_file_dialog->is_ok_snapshot_saver() ? Window::instance().m_file_dialog->get_file_path_snapshot_saver() : "";
            if (path != "") {
                Window::instance().rendering_mutex.unlock();
                Window::instance().take_screenshot(path);
                Window::instance().rendering_mutex.lock();
                Window::instance().m_file_dialog->close();
            }

        }
        ImGui::SameLine();
        HelpMarkerWithQuestionMark("With this Button you can use the Snapshot-function. It will open a file dialog, where you can "
                                   "choose a file in which you want to save your image of the actual Mesh");

        if (ImGui::CollapsingHeader("Selection")) {

            // Selection of single elements by typing in their ID
            const char* element_selection_types[] =
                    {
                            "Face", "Vertex", "Edge", "Cell"
                    };
            ImGui::Text("Manual Element Selection:");
            ImGui::Combo("  ", &m_manual_selection_type, element_selection_types, IM_ARRAYSIZE(element_selection_types), IM_ARRAYSIZE(element_selection_types));

            const char* selected_element_id_label = "ID";
            ImGui::InputInt(selected_element_id_label, &m_manual_selection_id);

            if(m_manual_selection_id != m_previous_manual_selection_id && m_manual_selection_id >= 0){
                // Unselect the previous manually selected element
                if(m_previous_manual_selection_id >= 0) {
                    Window::instance().rendering_mutex.unlock();
                    Window::instance().unselect_element(Window::instance().get_mesh_active(),
                                                        m_previous_manual_selection_id, m_previous_manual_selection_type);
                    Window::instance().rendering_mutex.lock();
                }
                // Select the new manually selected element
                Window::instance().rendering_mutex.unlock();
                Window::instance().select_element(Window::instance().get_mesh_active(), m_manual_selection_id, m_manual_selection_type);
                Window::instance().rendering_mutex.lock();
                m_previous_manual_selection_id = m_manual_selection_id;
                m_previous_manual_selection_type = m_manual_selection_type;
            }

            ImGui::Checkbox("Activate Click Selection",&m_selection_activated);
            GlobalViewerSettings::getInstance()->m_set_current_selection_activated(m_selection_activated);
            //ImGui::Checkbox("Vertex-Selection", &m_vertex_selection);
            if(m_selection_activated)
            {
                if (ImGui::RadioButton("All-Selection", m_current_selection_mode == 0))
                {
                    m_current_selection_mode = 0;
                    GlobalViewerSettings::getInstance()->m_set_current_selection_mode(0);
                }
                ImGui::SameLine(); HelpMarkerWithQuestionMark("This button will select the nearest Vertex, Edge or Face of your pick");
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
            }
        }
        //if (ImGui::Button("Selection"))
        //    ImGui::OpenPopup("Selection");
        if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted("By pushing this button diverse options for Selection of elements where shown. "
                                   "You can decide which one you want to use");
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }

        if(ImGui::CollapsingHeader("Mesh Settings"))
        {
            if(ImGui::BeginTable("split1", 1))
            {
                ImGui::TableNextColumn();
                int active_mesh = Window::instance().get_mesh_active();

                // Mesh transformations, such as position and scale
                if (active_mesh >= 0)
                {
                    auto mesh = Window::instance().get_mesh_obj(active_mesh);
                    auto pos = mesh->get_data().position;
                    auto scl = mesh->get_data().scale;
                    m_mesh_position[0] = pos.x;
                    m_mesh_position[1] = pos.y;
                    m_mesh_position[2] = pos.z;
                    m_mesh_scale = scl.x;
                }
                ImGui::Text("Position:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("Adjust the mesh position");
                if (ImGui::DragFloat3("##Position", m_mesh_position, 0.1f, -10.0f, 10.0f, "%.1f"))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().set_mesh_position(active_mesh, m_mesh_position[0], m_mesh_position[1], m_mesh_position[2]);
                        Window::instance().rendering_mutex.lock();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Reset"))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().set_mesh_position(active_mesh, 0.0f, 0.0f, 0.0f);
                        Window::instance().rendering_mutex.lock();
                    }
                }
                ImGui::Text("Scale:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("Adjust the mesh scale");
                if (ImGui::DragFloat("##Scale", &m_mesh_scale, 0.01f, 0.0f, 10.0f, "%.2f"))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().set_mesh_scale(active_mesh, m_mesh_scale);
                        Window::instance().rendering_mutex.lock();
                    }
                }
                ImGui::SameLine();

                // Push a new id for imgui, so we can use the same button label as before
                ImGui::PushID("ScaleReset");
                if (ImGui::Button("Reset"))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().set_mesh_scale(active_mesh, 1.0f);
                        Window::instance().rendering_mutex.lock();
                    }
                }
                ImGui::PopID();

                // Add some y padding and a separator
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y);
                // The default separator color is the same as the background, so change it here to something visible
                ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));
                ImGui::Separator();
                ImGui::PopStyleColor();

                // Mesh Filters
                ImGui::Text("Slicer:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("This slider will slice through the mesh to show an "
                                                              "inview of the mesh");
                m_slider_slicer = Window::instance().get_mesh_slice_level(active_mesh);
                m_slicer_locked = Window::instance().get_mesh_slice_locked(active_mesh);
                ImGui::SliderFloat("", &m_slider_slicer, 0.0f, 1.0f);
                ImGui::SameLine();
                ImGui::Checkbox("Lock", &m_slicer_locked);
                //GlobalViewerSettings::getInstance()->m_set_current_mesh_slice_level(m_slider_slicer);
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_slice_level(active_mesh,m_slider_slicer);
                Window::instance().set_mesh_slice_locked(active_mesh, m_slicer_locked);
                Window::instance().rendering_mutex.lock();
                ImGui::Text("Peel:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("This slider will peel the mesh like an onion");
                m_slider_peel = Window::instance().get_mesh_peel_level(active_mesh);
                int peel_max = 10;
                if (m_active_mesh >= 0)
                {
                    peel_max = Window::instance().get_mesh_obj(active_mesh)->get_max_peel_depth() + 1;
                }
                ImGui::SliderInt(" ", &m_slider_peel, 0, peel_max);
                //GlobalViewerSettings::getInstance()->m_set_current_mesh_peel_level(m_slider_peel);
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_peel_level(active_mesh,m_slider_peel);
                Window::instance().rendering_mutex.lock();
                m_cell_size = Window::instance().get_mesh_cell_size(active_mesh);
                ImGui::Text("Cell Size:");
                ImGui::SameLine(); HelpMarkerWithQuestionMark("This slider will change the size of each cell");
                if (ImGui::SliderFloat("##CellSize", &m_cell_size, 0.0f, 1.0f))
                {
                    //GlobalViewerSettings::getInstance()->m_set_current_cell_size(m_cell_size);
                    Window::instance().rendering_mutex.unlock();
                    Window::instance().set_mesh_cell_size(active_mesh,m_cell_size);
                    Window::instance().rendering_mutex.lock();
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

        
        /*
        if (ImGui::CollapsingHeader("Rendering Options"))
        {
            if (ImGui::BeginTable("split", 1))
            {
                // Decision on which mesh(es), the actual values should be used
                // Idea: iterate over meshes, for each mesh create a Selectable, which is represented by a bool. In Meshview: For each Mesh: check if bool is true -> change Color etc.
                ImGui::TableNextColumn();
                //ImGui::Text("   ");ImGui::SameLine();
                if(ImGui::Button("Mesh-Selection"))
                    showPopup1 = true;
                ImVec2 cursorPos = ImGui::GetCursorPos();
                ImGui::SetNextWindowPos(ImVec2(cursorPos.x + 100,cursorPos.y));
                if (ImGui::BeginPopup("Mesh-Selection"))
                {
                    int nbr_Meshes = GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes();
                    bool meshes[nbr_Meshes];
                    for (int i = 0; i < nbr_Meshes; i++) {
                        meshes[i] = GlobalViewerSettings::getInstance()->get_Visibility_of_Mesh(nbr_Meshes-1-i);
                    }
                    for (size_t i = 0; i < GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes(); i++)
                    {
                        const char* label = ("Mesh " + std::to_string(i+1)).c_str();
                        ImGui::Text("   ");ImGui::SameLine();
                        ImGui::Selectable(label,&meshes[i]);
                        //LogWindow::getInstance()->addLog("Hier2:");
                        //LogWindow::getInstance()->addLog(std::to_string(meshes[i]));
                    }

                    for (int i = 0; i < nbr_Meshes; i++) {
                        GlobalViewerSettings::getInstance()->set_Visibility_of_Mesh(nbr_Meshes - 1 - i, meshes[i]);
                    }

                    /*

                    std::vector<bool> arr = GlobalViewerSettings::getInstance()->get_test();
                    bool selected_mesh[arr.size()];
                    for (size_t i = 0; i < arr.size(); i++)
                    {
                        std::cout << "Hier gehts noch rein";
                        LogWindow::getInstance()->addLog("Hier:");
                        LogWindow::getInstance()->addLog(std::to_string(arr[i]));
                        selected_mesh[i] = arr[i];
                    }
                    
                    for (size_t i = 0; i < GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes(); i++)
                    {
                        const char* label = ("Mesh " + std::to_string(i+1)).c_str();
                        ImGui::Text("   ");ImGui::SameLine();
                        ImGui::Selectable(label,&selected_mesh[i]);
                        LogWindow::getInstance()->addLog("Hier2:");
                        LogWindow::getInstance()->addLog(std::to_string(selected_mesh[i]));
                    }

                    
                    std::vector<bool> newArr(GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes());
                    newArr.clear();
                    for (size_t i = 0; i < GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes(); i++)
                    {
                        newArr[i] = selected_mesh[i];
                    }
                    
                    GlobalViewerSettings::getInstance()->m_set_test(newArr);
                    if(ImGui::Button("Close"))
                    {
                        showPopup1 = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
                if (showPopup1)
                    ImGui::OpenPopup("Mesh-Selection");
                
/*
                if (ImGui::BeginPopup("Mesh-Selection"))
                {
                    for (size_t i = 0; i < m_nbr_meshes; i++)
                    {
                        const char* label = ("Mesh " + std::to_string(i)).c_str();
                        ImGui::Selectable(label,&m_selected_mesh[i]);
                    }
                    ImGui::EndPopup();
                }
                if (ImGui::Button("Mesh-Selcetion"))
                    ImGui::OpenPopup("Mesh-Selection");
                

                
                ImGui::Text("Color:");
                ImGui::Checkbox("", &m_color_activated);
                ImGui::SameLine();
                ImGui::ColorEdit4("", m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                if (m_color_activated)
                {
                    Window::instance().rendering_mutex.unlock();
                    Window::instance().set_mesh_color(Color(m_color[0],m_color[1],m_color[2],m_color[3]));
                    Window::instance().rendering_mutex.lock();
                }
                GlobalViewerSettings::getInstance()->m_set_current_mesh_rendering_color(m_color_activated, m_color[0],m_color[1],m_color[2],m_color[3]);
                ImGui::SameLine(); HelpMarkerWithQuestionMark(
                        "You can choose which color you want to use rendering the mesh");

                // Select an item type
                const char* rendering_mode_names[] =
                        {
                                "Phong", "Wireframe", "Normal", "Flat"
                        };
                const char* rendering_mode_internal_names[] =
                        {
                                "mesh_phong", "mesh_wireframe", "mesh_normal", "mesh_flat"
                        };
                ImGui::Text("Rendering Mode:");
                ImGui::Combo("  ", &m_rendering_mode, rendering_mode_names, IM_ARRAYSIZE(rendering_mode_names), IM_ARRAYSIZE(rendering_mode_names));
                // TODO
                Window::instance().rendering_mutex.unlock();
                //Window::instance().set_mesh_rendering_mode(rendering_mode_internal_names[m_rendering_mode]);
                Window::instance().rendering_mutex.lock();
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
        */
    /*
        if(ImGui::Button("Single Mesh Options"))
        {
            showPopup2 = true;
        }
        if (ImGui::BeginPopup("Single Mesh Options"))
        {
            int nbr_Meshes = GlobalViewerSettings::getInstance()->m_get_current_nbr_meeshes();
            if (nbr_Meshes < 1){

            }else {
                char *meshList[nbr_Meshes];
                for (int i = 0; i < nbr_Meshes; i++) {
                    std::string str = "Mesh " + std::to_string(i + 1);
                    char* char_type = new char[str.length()];
                    meshList[i] = strcpy(char_type, str.c_str());
                }
                //m_active_mesh = GlobalViewerSettings::getInstance()->m_get_current_active_mesh();
                m_active_mesh = Window::instance().get_mesh_active();
                ImGui::Combo("   ", &m_active_mesh, meshList, IM_ARRAYSIZE(meshList), IM_ARRAYSIZE(meshList));
                //GlobalViewerSettings::getInstance()->m_set_current_active_mesh(m_active_mesh);
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_active(m_active_mesh);
                Window::instance().rendering_mutex.lock();

                bool visible = Window::instance().get_mesh_visibility(m_active_mesh);
                ImGui::Checkbox("Visible", &visible);
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_visibility(m_active_mesh, visible);
                Window::instance().rendering_mutex.lock();

                Color color = Window::instance().get_mesh_color(m_active_mesh);
                m_color[0] = color.get().r;
                m_color[1] = color.get().g;
                m_color[2] = color.get().b;
                ImGui::ColorEdit4("", m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_color(m_active_mesh, Color(m_color[0], m_color[1], m_color[2], m_color[3]));
                Window::instance().rendering_mutex.lock();

                std::string current_rendering_mode = Window::instance().get_mesh_rendering_mode(m_active_mesh);
                int current_rendering_mode_int = 0;
                if (current_rendering_mode == "mesh_wireframe")
                    current_rendering_mode_int = 1;
                if (current_rendering_mode == "mesh_normal")
                    current_rendering_mode_int = 2;
                if (current_rendering_mode == "mesh_flat")
                    current_rendering_mode_int = 3;
                const char *rendering_mode_internal_names[] =
                        {
                                "mesh_phong", "mesh_wireframe", "mesh_normal", "mesh_flat"
                        };
                ImGui::Text("Rendering Mode:");
                ImGui::Combo("  ", &current_rendering_mode_int, rendering_mode_internal_names,
                             IM_ARRAYSIZE(rendering_mode_internal_names), IM_ARRAYSIZE(rendering_mode_internal_names));
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_rendering_mode(m_active_mesh,rendering_mode_internal_names[current_rendering_mode_int]);
                Window::instance().rendering_mutex.lock();
            }
            if(ImGui::Button("Close"))
            {
                showPopup2 = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        if (showPopup2)
            ImGui::OpenPopup("Single Mesh Options");
        */
    ImGui::End();
    }
} // namespace vOS
