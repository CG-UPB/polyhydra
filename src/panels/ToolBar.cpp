
#include "ToolBar.h"
#include "../input/Input.h"
#include "MeshView.h"
#include "NewFileDialog.h"
#include "../util/Tooltips.h"
#include "../util/ImGuiUtil.h"
#include "volumeshOS.h"


namespace volumeshOS::Internal
{

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
                volumeshOS::export_image(filename);
            }
        }

        ImGuiUtil::add_padding_y(0.5f);

        ImGui::Text("Rendering");
        ImGui::SetNextItemWidth(slider_width);
        ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);

        int rendering_mode = static_cast<int>(AppState::settings.rendering_mode);
        const char *element_mode_types[] =
                {
                        "Wireframe",
                        "Only Vertices",
                        "Phong Facenormals",
                        "Phong Vertexnormals"
                };
        ImGui::Combo("##Manual Mode SelectionMode:", &rendering_mode, element_mode_types,
                     IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
        AppState::settings.rendering_mode = static_cast<RenderingMode>(rendering_mode);

        ImGuiUtil::add_padding_y(0.5f);
        ImGui::Separator();
        ImGuiUtil::add_padding_y(0.5f);
        ImGuiUtil::push_bold_font();
        ImGui::Text("SelectionMode");
        ImGui::PopFont();
        ImGuiUtil::add_padding_y(0.5f);

        const char *selection_modes[] =
                {
                        "OFF", "Vertices", "Edges", "Halffaces", "Cells", "All"
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
        AppState::settings.selection_mode = static_cast<SelectionMode>(m_current_selection_mode);
        AppState::settings.selection_active = AppState::settings.selection_mode != SelectionMode::OFF;

        // SelectionMode-variables are set here
        // SelectionMode of single elements by typing in their ID
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
//            if (m_previous_manual_selection_id >= 0)
//            {
//                Window::instance().rendering_mutex.unlock();
//                Window::instance().unselect_element(Window::instance().get_mesh_focus(),
//                                                    m_previous_manual_selection_id,
//                                                    m_previous_manual_selection_type);
//                Window::instance().rendering_mutex.lock();
//                =
//            }
//            // Select the new manually selected element
//            Window::instance().rendering_mutex.unlock();
//            Window::instance().select_element(Window::instance().get_mesh_focus(), m_manual_selection_id,
//                                              m_manual_selection_type);
//            Window::instance().rendering_mutex.lock();
            m_previous_manual_selection_id = m_manual_selection_id;
            m_previous_manual_selection_type = m_manual_selection_type;
        }

        auto active_mesh = volumeshOS::get_focused_mesh();

        // If there is at least one mesh, the Active Mesh Settings (Slicing, Peeling, etc.) are available
        if (active_mesh.is_valid())
        {
            ImGuiUtil::add_padding_y(0.5f);
            ImGui::Separator();
            ImGuiUtil::add_padding_y(0.5f);
            ImGuiUtil::push_bold_font();
            ImGui::Text("%s", active_mesh.get_name().c_str());
            ImGui::PopFont();
            ImGuiUtil::add_padding_y(0.5f);

            if (ImGui::BeginTable("split1", 1))
            {

                ImGui::TableNextColumn();
                // Mesh transformations, such as position and scale
                auto pos = active_mesh.get_position();
                auto scl = active_mesh.get_scale();
                m_mesh_position[0] = pos[0];
                m_mesh_position[1] = pos[1];
                m_mesh_position[2] = pos[2];
                m_mesh_scale = scl;

                ImGui::Text("Position");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh position");
                ImGui::SetNextItemWidth(slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::DragFloat3("##Position", m_mesh_position, 0.1f, -10.0f, 10.0f, "%.1f"))
                {
                    active_mesh.set_position(m_mesh_position[0], m_mesh_position[1], m_mesh_position[2]);
                }
                ImGui::SameLine();
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    active_mesh.set_position(0.0f, 0.0f, 0.0f);
                }
                ImGui::Separator();
                ImGui::Text("Scale");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh scale");
                ImGui::SetNextItemWidth(slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::DragFloat("##Scale", &m_mesh_scale, 0.01f, 0.0f, 10.0f, "%.2f"))
                {
                    active_mesh.set_scale(m_mesh_scale);
                }
                ImGui::SameLine();

                // Push a new id for imgui, so we can use the same button label as before
                ImGui::PushID("ScaleReset");
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    active_mesh.set_scale(1.0f);
                }
                ImGui::PopID();

                ImGui::Separator();

                // Mesh Filters
                ImGui::Text("Slicer");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will slice through the mesh to show an "
                                                     "inview of the mesh");
                m_slider_slicer = active_mesh.get_slice_factor();
                m_slicer_locked = active_mesh.get_slice_lock();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("", &m_slider_slicer, 0.0f, 1.0f);
                ImGui::Text(" ");
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::Checkbox("Lock", &m_slicer_locked);
                active_mesh.set_slice_factor(m_slider_slicer);
                active_mesh.set_slice_lock(m_slicer_locked);
                ImGui::Separator();
                ImGui::Text("Peel");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will peel the mesh like an onion");
                m_slider_peel = active_mesh.get_peel_level();
                float peel_max = (float) active_mesh.get_max_peel_depth() + 1.0f;

                // make it easier to get the slider onto an Integer
                // thats helpful for peeling with transparent transition
                float tolerance = 0.05;
                if((int)(m_slider_peel + tolerance) != (int)(m_slider_peel - tolerance))
                {
                    m_slider_peel = (float)(int)(m_slider_peel + tolerance);
                }
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if( ImGui::SliderFloat(" ", &m_slider_peel, 0, peel_max))
                {
                    active_mesh.set_peel_level(m_slider_peel);
                }

                m_cell_size = active_mesh.get_cell_size();
                ImGui::Separator();
                ImGui::Text("Cell Size");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will change the size of each cell");
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##CellSize", &m_cell_size, 0.0f, 1.0f))
                {
                    active_mesh.set_cell_size(m_cell_size);
                }
                ImGui::Separator();
                ImGui::Text("Roundings");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This checkbox activates rounded corners for the edges of the meshes");
                float actual_rounding_size = active_mesh.get_cell_rounding();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("Size", &actual_rounding_size, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
                {
                    active_mesh.set_cell_rounding(actual_rounding_size);
                    active_mesh.activate_rounding((actual_rounding_size != 0.0f));
                }

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
                    active_mesh.reset_visibility();
                }
                ImGui::SameLine();
                if (ImGui::Button(m_digging_activated ? "Deactivate" : "Activate"))
                {
                    if (!m_digging_activated)
                    {
                        m_digging_activated = true;
                        m_current_selection_mode = static_cast<int>(SelectionMode::CELL);
                        AppState::settings.selection_active = true;
                        AppState::settings.selection_mode = SelectionMode::CELL;
                    }
                    else
                    {
                        m_digging_activated = false;
                    }
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
                        m_current_selection_mode = static_cast<int>(SelectionMode::CELL);
                        AppState::settings.selection_active = true;
                        AppState::settings.selection_mode = SelectionMode::CELL;
                    }
                    else
                    {
                        m_isolation_started = false;
                        active_mesh.reset_visibility();
                    }
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
