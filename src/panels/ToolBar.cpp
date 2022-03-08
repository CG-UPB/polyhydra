#include "ToolBar.h"
#include "../input/Input.h"
#include "imgui.h"
#include "MeshView.h"
#include "NewFileDialog.h"
#include "../util/Tooltips.h"


namespace vOS
{


    ToolBar::ToolBar() {
    }

    // Destruktor
    ToolBar::~ToolBar() {
        //delete instance;
    }


    void ToolBar::show() {
            if (!ImGui::Begin("Toolbox")) {
                ImGui::End();
                return;
            }

            int mesh_mode = GlobalViewerSettings::getInstance()->m_get_current_mesh_mode();

            const char *element_mode_types[] =
            {
                    "Wireframe",
                    "Only Vertices",
                    "Phong Facenormals",
                    "Phong Vertexnormals",
                    "Transparency",
                    "Rounded",
                    "Ambient Occlusion",
                    "Shadows"
            };
            ImGui::Text("Manual Mode Selection:");
            ImGui::Combo("  ", &mesh_mode, element_mode_types,
                         IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("Here you can choose which of our modes you want to use");
            GlobalViewerSettings::getInstance()->m_set_current_mesh_mode(mesh_mode);

            // Snapshot Button uses the class Filedialog to save a screenshot
            if (ImGui::Button("Snapshot")) {
                NewFileDialog file_dialog;

                char const *filename;

                filename = file_dialog.saveDialog("Open Snapshot File");

                if (filename != NULL)
                {
                    Window::instance().rendering_mutex.unlock();
                    Window::instance().take_screenshot(filename);
                    Window::instance().rendering_mutex.lock();
                }
            }
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark(
                    "With this Button you can use the Snapshot-function. It will open a file dialog, where you can "
                    "choose a file in which you want to save your image of the actual Mesh");


            // Selection-variables are set here
           if (ImGui::CollapsingHeader("Selection")) {

               // Selection of single elements by typing in their ID
               const char *element_selection_types[] =
                       {
                               "Face", "Vertex", "Edge", "Cell"
                       };
               ImGui::Text("Manual Element Selection:");
               ImGui::Combo("  ", &m_manual_selection_type, element_selection_types,
                            IM_ARRAYSIZE(element_selection_types), IM_ARRAYSIZE(element_selection_types));
               ImGui::SameLine();
               Tooltips::HelpMarkerWithQuestionMark("Here you can choose which of the elements you want to select");
               const char *selected_element_id_label = "ID";
               ImGui::InputInt(selected_element_id_label, &m_manual_selection_id);
               ImGui::SameLine();
               Tooltips::HelpMarkerWithQuestionMark("Input the id here!");
               if (m_manual_selection_id != m_previous_manual_selection_id && m_manual_selection_id >= 0) {
                   // Unselect the previous manually selected element
                   if (m_previous_manual_selection_id >= 0) {
                       Window::instance().rendering_mutex.unlock();
                       Window::instance().unselect_element(Window::instance().get_mesh_focus(),
                                                           m_previous_manual_selection_id,
                                                           m_previous_manual_selection_type);
                       Window::instance().rendering_mutex.lock();
                   }
                   // Select the new manually selected element
                   Window::instance().rendering_mutex.unlock();
                   Window::instance().select_element(Window::instance().get_mesh_focus(), m_manual_selection_id,
                                                     m_manual_selection_type);
                   Window::instance().rendering_mutex.lock();
                   m_previous_manual_selection_id = m_manual_selection_id;
                   m_previous_manual_selection_type = m_manual_selection_type;
               }

               ImGui::Checkbox("Activate Click Selection", &m_selection_activated);
               ImGui::SameLine();
               Tooltips::HelpMarkerWithQuestionMark("With this checkbox you can activate the click selection");
               GlobalViewerSettings::getInstance()->m_set_current_selection_activated(m_selection_activated);
               if (m_selection_activated) {
                   if (ImGui::RadioButton("All-Selection", m_current_selection_mode == 0)) {
                       m_current_selection_mode = 0;
                       GlobalViewerSettings::getInstance()->m_set_current_selection_mode(0);
                   }
                   ImGui::SameLine();
                   Tooltips::HelpMarkerWithQuestionMark(
                           "This button will select the nearest Vertex, Edge or Face of your pick");
                   if (ImGui::RadioButton("Vertex-Selection", m_current_selection_mode == Vertex)) {
                       m_current_selection_mode = Vertex;
                       GlobalViewerSettings::getInstance()->m_set_current_selection_mode(Vertex);
                   }
                   ImGui::SameLine();
                   Tooltips::HelpMarkerWithQuestionMark("This button will select the nearest Vertex of your pick");
                   if (ImGui::RadioButton("Edge-Selection", m_current_selection_mode == Edge)) {
                       m_current_selection_mode = Edge;
                       GlobalViewerSettings::getInstance()->m_set_current_selection_mode(Edge);
                   }
                   ImGui::SameLine();
                   Tooltips::HelpMarkerWithQuestionMark("This button will select the nearest Edge of your pick");
                   if (ImGui::RadioButton("Face-Selection", m_current_selection_mode == Face)) {
                       m_current_selection_mode = Face;
                       GlobalViewerSettings::getInstance()->m_set_current_selection_mode(Face);
                   }
                   ImGui::SameLine();
                   Tooltips::HelpMarkerWithQuestionMark("This button will select the nearest Face of your pick");
                   if (ImGui::RadioButton("Cell-Selection", m_current_selection_mode == CELL)) {
                       m_current_selection_mode = CELL;
                       GlobalViewerSettings::getInstance()->m_set_current_selection_mode(CELL);
                   }
               }
           }
           Tooltips::ToolTipByHovering("By pushing this button diverse options for Selection of elements where shown. "
                                       "You can decide which one you want to use");

            int active_mesh = Window::instance().get_mesh_focus();

            // If there is at least one mesh, the Active Mesh Settings (Slicing, Peeling, etc.) are available
            if (active_mesh != -1) {
                if (ImGui::CollapsingHeader("Active Mesh Settings")) {
                    if (ImGui::BeginTable("split1", 1)) {


                        ImGui::TableNextColumn();
                        // Mesh transformations, such as position and scale
                        if (active_mesh >= 0) {
                            auto mesh = Window::instance().get_mesh_obj(active_mesh);
                            auto pos = mesh->get_data().m_position;
                            auto scl = mesh->get_data().m_scale;
                            m_mesh_position[0] = pos.x;
                            m_mesh_position[1] = pos.y;
                            m_mesh_position[2] = pos.z;
                            m_mesh_scale = scl.x;
                        }


                        ImGui::Text("Position:");
                        ImGui::SameLine();
                        Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh position");
                        if (ImGui::DragFloat3("##Position", m_mesh_position, 0.1f, -10.0f, 10.0f, "%.1f")) {
                            if (active_mesh >= 0) {
                                Window::instance().set_mesh_position(active_mesh, m_mesh_position[0],
                                                                     m_mesh_position[1],
                                                                     m_mesh_position[2]);
                            }
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Reset")) {
                            if (active_mesh >= 0) {
                                Window::instance().set_mesh_position(active_mesh, 0.0f, 0.0f, 0.0f);
                            }
                        }
                        ImGui::Text("Scale:");
                        ImGui::SameLine();
                        Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh scale");
                        if (ImGui::DragFloat("##Scale", &m_mesh_scale, 0.01f, 0.0f, 10.0f, "%.2f")) {
                            if (active_mesh >= 0) {
                                Window::instance().set_mesh_scale(active_mesh, m_mesh_scale);
                            }
                        }
                        ImGui::SameLine();

                        // Push a new id for imgui, so we can use the same button label as before
                        ImGui::PushID("ScaleReset");
                        if (ImGui::Button("Reset")) {
                            if (active_mesh >= 0) {
                                Window::instance().set_mesh_scale(active_mesh, 1.0f);
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
                        ImGui::SameLine();
                        Tooltips::HelpMarkerWithQuestionMark("This slider will slice through the mesh to show an "
                                                             "inview of the mesh");
                        m_slider_slicer = Window::instance().get_mesh_slice_level(active_mesh);
                        m_slicer_locked = Window::instance().get_mesh_slice_locked(active_mesh);
                        ImGui::SliderFloat("", &m_slider_slicer, 0.0f, 1.0f);
                        ImGui::SameLine();
                        ImGui::Checkbox("Lock", &m_slicer_locked);
                        Window::instance().set_mesh_slice_level(active_mesh, m_slider_slicer);
                        Window::instance().set_mesh_slice_locked(active_mesh, m_slicer_locked);
                        ImGui::Text("Peel:");
                        ImGui::SameLine();
                        Tooltips::HelpMarkerWithQuestionMark("This slider will peel the mesh like an onion");
                        m_slider_peel = Window::instance().get_mesh_peel_level(active_mesh);
                        int peel_max = 10;
                        if (m_active_mesh >= 0) {
                            peel_max = Window::instance().get_mesh_obj(active_mesh)->get_max_peel_depth() + 1;
                        }
                        ImGui::SliderInt(" ", &m_slider_peel, 0, peel_max);
                        Window::instance().set_mesh_peel_level(active_mesh, m_slider_peel);
                        m_cell_size = Window::instance().get_mesh_cell_size(active_mesh);
                        ImGui::Text("Cell Size:");
                        ImGui::SameLine();
                        Tooltips::HelpMarkerWithQuestionMark("This slider will change the size of each cell");
                        if (ImGui::SliderFloat("##CellSize", &m_cell_size, 0.0f, 1.0f)) {
                            Window::instance().set_mesh_cell_size(active_mesh, m_cell_size);
                        }

                        bool rounding_active = GlobalViewerSettings::getInstance()->m_get_current_rounding_active();

                        ImGui::Checkbox("Rounded Cells?", &rounding_active);
                        GlobalViewerSettings::getInstance()->m_set_current_rounding_active(rounding_active);
                        if (rounding_active)
                        {
                            float actual_rounding_size = GlobalViewerSettings::getInstance()->m_get_current_rounding_size();
                            ImGui::SliderFloat("Size", &actual_rounding_size, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
                            GlobalViewerSettings::getInstance()->m_set_current_rounding_size(actual_rounding_size);
                        }

                        static int clicked_digging = 0;
                        if(ImGui::Button("Activate Digging"))
                        {
                            if (!m_digging_activated) {
                                m_digging_activated = true;
                                GlobalViewerSettings::getInstance()->m_set_current_digging_active(m_digging_activated);
                                clicked_digging++;
                            }
                        }
                        if (clicked_digging & 1) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Digging is Active");
                            if (ImGui::Button("Reset Digging")) {
                                if (m_digging_activated) {
                                    m_digging_activated = false;
                                    GlobalViewerSettings::getInstance()->m_set_current_digging_active(m_digging_activated);
                                    clicked_digging++;
                                }
                            }
                        }

                        // Therefore, a picker has to work
                        ImGui::Text("Start Isolation:");
                        ImGui::SameLine();
                        Tooltips::HelpMarkerWithQuestionMark("With this button you can start a selection of a "
                                                             "variable number of vertices, edges or faces, which you "
                                                             "want to visualize for their own. To stop the selection "
                                                             "you have to press the button below");
                        static int clicked = 0;
                        if (ImGui::Button("Isolationstart")) {
                            if (!m_isolation_started) {
                                m_isolation_started = true;
                                GlobalViewerSettings::getInstance()->m_set_current_isolation_state(m_isolation_started);
                                clicked++;
                            }
                        }
                        if (clicked & 1) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Isolation-Selection is Active");
                        }
                        //...
                        ImGui::Text("Finish Isolation:");
                        if (ImGui::Button("Isolationend")) {
                            if (m_isolation_started) {
                                m_isolation_started = false;
                                GlobalViewerSettings::getInstance()->m_set_current_isolation_state(m_isolation_started);
                                for (const auto& m: Window::instance().get_mesh_list())
                                {
                                    auto mesh = m.second;
                                    auto mvb = mesh->get_mvb();
                                    mvb->activate_isolation();
                                }
                                clicked++;
                            }
                        }

                        ImGui::EndTable();
                    }
                }
                Tooltips::ToolTipByHovering("By pushing this button diverse options for Filtering the mesh where shown. "
                                            "There are multiple options to get a better view of the mesh");
            }
            ImGui::End();
        }

} // namespace vOS
