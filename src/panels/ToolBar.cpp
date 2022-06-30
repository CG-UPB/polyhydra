
#include "ToolBar.h"
#include "../input/Input.h"
#include "MeshView.h"
#include "NewFileDialog.h"
#include "../util/Tooltips.h"
#include "../util/ImGuiUtil.h"


namespace volumeshOS::Internal
{


    ToolBar::ToolBar()
    {
    }

    // Destruktor
    ToolBar::~ToolBar()
    {
        //delete instance;
    }


    void ToolBar::show()
    {
        if (!ImGui::Begin("Toolbar"))
        {
            ImGui::End();
            return;
        }

        float slider_width = 180.0f;
        float padding_right = 20.0f;

        ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));

        ImGuiUtil::push_bold_font();
        ImGui::Text("Global");
        ImGui::PopFont();

        ImGui::SameLine(ImGui::GetWindowWidth() - 38.0f - padding_right);

        // Snapshot Button uses the class Filedialog to save a screenshot
        if (ImGuiUtil::icon_button("camera.png"))
        {
            NewFileDialog file_dialog;

            char const *filename;

            filename = file_dialog.save_dialog("Open Snapshot File");

            if (filename != nullptr)
            {
                Window::instance().rendering_mutex.unlock();
                Window::instance().take_screenshot(filename);
                Window::instance().rendering_mutex.lock();
            }
        }

        ImGuiUtil::add_padding_y(0.5f);

        ImGui::Text("Rendering");
        ImGui::SetNextItemWidth(slider_width);
        ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);

        int mesh_mode = GlobalViewerSettings::getInstance()->get_mesh_mode();
        int old_mode = mesh_mode;
        const char *element_mode_types[] =
                {
                        "Wireframe",
                        "Only Vertices",
                        "Phong Facenormals",
                        "Phong Vertexnormals"
                };
        ImGui::Combo("##Manual Mode Selection:", &mesh_mode, element_mode_types,
                     IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
//        Tooltips::HelpMarkerWithQuestionMark("Here you can choose which of our modes you want to use. For more "
//                                             "extensive explanations take a look in the documentation");
        GlobalViewerSettings::getInstance()->set_mesh_mode(mesh_mode);

        ImGuiUtil::add_padding_y(0.5f);
        ImGui::Separator();
        ImGuiUtil::add_padding_y(0.5f);
        ImGuiUtil::push_bold_font();
        ImGui::Text("Selection");
        ImGui::PopFont();
        ImGuiUtil::add_padding_y(0.5f);

        const char *selection_modes[] =
                {
                        "Off", "Vertices", "Edges", "Halffaces", "Cells", "All"
                };

        ImGui::Text("Mode");
        ImGui::SetNextItemWidth(slider_width);
        ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
        ImGui::Combo(
                "##SelectionMode",
                &m_current_selection_mode,
                selection_modes,
                IM_ARRAYSIZE(selection_modes),
                IM_ARRAYSIZE(selection_modes)
        );
        GlobalViewerSettings::getInstance()->set_selection_mode(m_current_selection_mode);
        GlobalViewerSettings::getInstance()->set_selection_activated(m_current_selection_mode != Selection::Off);

        // Selection-variables are set here
        // Selection of single elements by typing in their ID
        const char *element_selection_types[] =
                {
                        "Face", "Vertex", "Edge", "Cell"
                };
        ImGui::Text("Select by ID");
        ImGui::SetNextItemWidth(slider_width);
        ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
        ImGui::Combo("  ", &m_manual_selection_type, element_selection_types,
                     IM_ARRAYSIZE(element_selection_types), IM_ARRAYSIZE(element_selection_types));
        ImGui::Text(" ");
        ImGui::SetNextItemWidth(slider_width);
        ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
        ImGui::InputInt("##ManualSelectionID", &m_manual_selection_id);
        if (m_manual_selection_id != m_previous_manual_selection_id && m_manual_selection_id >= 0)
        {
            // Unselect the previous manually selected element
            if (m_previous_manual_selection_id >= 0)
            {
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

        int active_mesh = Window::instance().get_mesh_focus();

        // If there is at least one mesh, the Active Mesh Settings (Slicing, Peeling, etc.) are available
        if (active_mesh != -1)
        {
            std::string header_name = "Mesh " + std::to_string(active_mesh);

            ImGuiUtil::add_padding_y(0.5f);
            ImGui::Separator();
            ImGuiUtil::add_padding_y(0.5f);
            ImGuiUtil::push_bold_font();
            ImGui::Text("%s", header_name.c_str());
            ImGui::PopFont();
            ImGuiUtil::add_padding_y(0.5f);

            if (ImGui::BeginTable("split1", 1))
            {

                ImGui::TableNextColumn();
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

                ImGui::Text("Position");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh position");
                ImGui::SetNextItemWidth(slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::DragFloat3("##Position", m_mesh_position, 0.1f, -10.0f, 10.0f, "%.1f"))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().set_mesh_position(active_mesh, m_mesh_position[0],
                                                             m_mesh_position[1],
                                                             m_mesh_position[2]);
                    }
                }
                ImGui::SameLine();
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().set_mesh_position(active_mesh, 0.0f, 0.0f, 0.0f);
                    }
                }
                ImGui::Separator();
                ImGui::Text("Scale");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh scale");
                ImGui::SetNextItemWidth(slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::DragFloat("##Scale", &m_mesh_scale, 0.01f, 0.0f, 10.0f, "%.2f"))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().set_mesh_scale(active_mesh, m_mesh_scale);
                    }
                }
                ImGui::SameLine();

                // Push a new id for imgui, so we can use the same button label as before
                ImGui::PushID("ScaleReset");
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    if (active_mesh >= 0)
                    {
                        Window::instance().set_mesh_scale(active_mesh, 1.0f);
                    }
                }
                ImGui::PopID();

                ImGui::Separator();

                // Mesh Filters
                ImGui::Text("Slicer");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will slice through the mesh to show an "
                                                     "inview of the mesh");
                m_slider_slicer = Window::instance().get_mesh_slice_level(active_mesh);
                m_slicer_locked = Window::instance().get_mesh_slice_locked(active_mesh);
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("", &m_slider_slicer, 0.0f, 1.0f);
                ImGui::Text(" ");
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::Checkbox("Lock", &m_slicer_locked);
                Window::instance().set_mesh_slice_level(active_mesh, m_slider_slicer);
                Window::instance().set_mesh_slice_locked(active_mesh, m_slicer_locked);
                ImGui::Separator();
                ImGui::Text("Peel");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will peel the mesh like an onion");
                m_slider_peel = Window::instance().get_mesh_peel_level(active_mesh);
                float peel_max = 0.0f;
                if (m_active_mesh >= 0)
                {
                    peel_max = (float)Window::instance().get_mesh_obj(active_mesh)->get_max_peel_depth() + 1.0f;
                }

                // make it easier to get the slider onto an Integer
                // thats helpful for peeling with transparent transition
                float tolerance = 0.05;
                if((int)(m_slider_peel + tolerance) != (int)(m_slider_peel - tolerance))
                {
                    m_slider_peel = (float)(int)(m_slider_peel + tolerance);
                }
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat(" ", &m_slider_peel, 0, peel_max);

                Window::instance().set_mesh_peel_level(active_mesh, m_slider_peel);
                m_cell_size = Window::instance().get_mesh_cell_size(active_mesh);
                ImGui::Separator();
                ImGui::Text("Cell Size");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will change the size of each cell");
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##CellSize", &m_cell_size, 0.0f, 1.0f))
                {
                    Window::instance().set_mesh_cell_size(active_mesh, m_cell_size);
                }
                ImGui::Separator();
                ImGui::Text("Roundings");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This checkbox activates rounded corners for the edges of the meshes");
                float actual_rounding_size = Window::instance().get_mesh_rounding_size(active_mesh);
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("Size", &actual_rounding_size, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
                Window::instance().set_mesh_rounding_size(active_mesh, actual_rounding_size);
                Window::instance().set_mesh_rounding_activated(active_mesh,actual_rounding_size > 0.0f);
                ImGui::Separator();
                ImGui::Text("Digging");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Dig into the mesh by clicking cells to remove");

                static int clicked_digging = 0;
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::PushID("Digging");
                if (ImGui::Button("Reset"))
                {
                    if (active_mesh >= 0)
                    {
                        auto mesh = Window::instance().get_mesh_obj(active_mesh);
                        mesh->get_mvb()->reset_digging();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button(m_digging_activated ? "Deactivate" : "Activate"))
                {
                    if (!m_digging_activated)
                    {
                        m_digging_activated = true;
                        m_selection_activated = true;
                        m_current_selection_mode = Selection::CELL;
                        GlobalViewerSettings::getInstance()->set_selection_activated(true);
                        GlobalViewerSettings::getInstance()->set_selection_mode(Selection::CELL);
                    }
                    else
                    {
                        m_digging_activated = false;
                    }
                    GlobalViewerSettings::getInstance()->set_digging_active(m_digging_activated);
                    clicked_digging++;
                }
                if (clicked_digging & 1)
                {
                    //ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Active");
                }
                ImGui::PopID();
                ImGui::Separator();
                ImGui::Text("Isolation");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Isolate a single cell by clicking on it. Click again to reset the isolation");

                static int clicked = 0;
                ImGui::PushID("Isolation");
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::Button(m_isolation_started ? "Deactivate" : "Activate"))
                {
                    if (!m_isolation_started)
                    {
                        m_isolation_started = true;
                        m_selection_activated = true;
                        m_current_selection_mode = Selection::CELL;
                        GlobalViewerSettings::getInstance()->set_selection_activated(true);
                        GlobalViewerSettings::getInstance()->set_selection_mode(Selection::CELL);
                    }
                    else
                    {
                        if (active_mesh >= 0)
                        {
                            m_isolation_started = false;
                            auto mesh = Window::instance().get_mesh_obj(active_mesh);
                            mesh->get_mvb()->reset_isolation();
                        }
                    }
                    GlobalViewerSettings::getInstance()->set_isolation_state(m_isolation_started);
                    clicked++;
                }
                if (clicked & 1)
                {
                    //ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Active");
                }
                ImGui::PopID();
                ImGui::Separator();
                ImGui::EndTable();
            }
        }
        ImGui::PopStyleColor();
        ImGui::End();
    }

} // namespace volumeshOS
