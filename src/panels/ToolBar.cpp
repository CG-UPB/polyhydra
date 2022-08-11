
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


        ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));
        ImGui::SetScrollX(1);

        ImGui::Separator();
        ImGuiUtil::push_bold_font();
        ImGui::Text("Settings");
        ImGui::PopFont();


        ImGui::SameLine(ImGui::GetWindowWidth() - 38.0f - padding_right - 2.0f);

        // Snapshot Button uses the class Filedialog to save a screenshot
        if (ImGuiUtil::icon_button("camera.png"))
        {
            NewFileDialog file_dialog;

            char const* filename;

            filename = file_dialog.save_dialog("Open Snapshot File");

            if (filename != nullptr)
            {
                volumeshOS::export_image(filename);
            }
        }

        /* ##########  TEST  ############*/

        show_rendering_mode_menu();
        show_selection_menu();
        show_camera_menu();

        ImGui::Separator();
        ImGuiUtil::push_bold_font();
        ImGui::Text("Graphics");
        ImGui::PopFont();

        ImGui::DragFloat("Gamma", &AppState::settings.gamma, 0.1f, 1.0f, 4.0f, "%.1f");

        show_ground_menu();
        show_shadow_menu();
        show_ambient_occlusion_menu();
        show_transparency_menu();

        ImGui::Separator();
        ImGuiUtil::push_bold_font();
        ImGui::Text("Mesh List");
        ImGui::PopFont();
//        show_mesh_list();

        /* ##########  TEST  ############*/

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
                auto pos = active_mesh.get_position<glm::vec3>();
                auto scl = active_mesh.get_scale();
                auto rot = active_mesh.get_rotation<glm::vec3>();
                m_mesh_position[0] = pos[0];
                m_mesh_position[1] = pos[1];
                m_mesh_position[2] = pos[2];
                m_mesh_scale = scl;
                m_mesh_rotation[0] = glm::degrees(rot[0]);
                m_mesh_rotation[1] = glm::degrees(rot[1]);
                m_mesh_rotation[2] = glm::degrees(rot[2]);


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

                ImGui::Text("Rotation");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("Adjust the mesh rotation");
                ImGui::SetNextItemWidth(slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::DragFloat3("##Rotation", m_mesh_rotation, 1.0f, -180.0f, 180.0f, "%.1f"))
                {
                    auto x = (m_mesh_rotation[0] + 180.0f) - glm::degrees(rot[0]) + 180.0f;
                    auto y = (m_mesh_rotation[1] + 180.0f) - glm::degrees(rot[1]) + 180.0f;
                    auto z = (m_mesh_rotation[2] + 180.0f) - glm::degrees(rot[2]) + 180.0f;
                    auto epsilon = 0.01;

                    if (x >= epsilon || y >= epsilon || z >= epsilon)
                    {
                        active_mesh.set_rotation(glm::radians(x), glm::radians(y), glm::radians(z));
                    }

                }
                ImGui::SameLine();
                ImGui::PushID("RotationReset");
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    active_mesh.reset_rotation();
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
                if ((int) (m_slider_peel + tolerance) != (int) (m_slider_peel - tolerance))
                {
                    m_slider_peel = (float) (int) (m_slider_peel + tolerance);
                }
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat(" ", &m_slider_peel, 0, peel_max))
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
                Tooltips::HelpMarkerWithQuestionMark(
                        "This checkbox activates rounded corners for the edges of the meshes");
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
                    AppState::settings.digging_active = m_digging_activated;
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
                Tooltips::HelpMarkerWithQuestionMark(
                        "Isolate a single cell by clicking on it. Click again to reset the isolation");

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
                    AppState::settings.isolation_active = m_isolation_started;
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

    void ToolBar::show_rendering_mode_menu()
    {
        if (!ImGui::CollapsingHeader("Rendering Modes"))
        {
            return;
        }
        shift_right();

        ImGui::SetNextItemWidth(slider_width);
        //ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);

        int rendering_mode = static_cast<int>(AppState::settings.rendering_mode);
        const char* element_mode_types[] =
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
    }


    void ToolBar::show_selection_menu()
    {
        auto& settings = AppState::settings;

        if (!ImGui::CollapsingHeader("Selection"))
        {
            return;
        }
        shift_right();

        const char* selection_modes[] =
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
        const char* element_selection_types[] =
                {
                        "Face", "Vertex", "Edge", "Cell"
                };
        shift_right();
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
            m_previous_manual_selection_id = m_manual_selection_id;
            m_previous_manual_selection_type = m_manual_selection_type;
        }


    }

    void ToolBar::show_camera_menu()
    {
        auto& settings = AppState::settings;

        if (!ImGui::CollapsingHeader("Camera"))
        {
            return;
        }
        shift_right();
    }


    void ToolBar::show_ground_menu()
    {
        auto& settings = AppState::settings;
        bool visible = settings.ground_options.visible;
        bool solid = AppState::settings.ground_options.solid;
        glm::vec3 solid_color = AppState::settings.ground_options.solid_color;
        bool grid = AppState::settings.ground_options.grid;
        glm::vec3 grid_color = AppState::settings.ground_options.grid_color;
        float height = AppState::settings.ground_options.height;
        float new_color1[4];
        new_color1[0] = solid_color.r;
        new_color1[1] = solid_color.g;
        new_color1[2] = solid_color.b;



        if(ImGui::Checkbox("###Ground", &visible))
        {
            settings.ground_options.visible = visible;
        }
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Ground"))
        {
            return;
        }

        shift_right();
        if (ImGui::ColorEdit3("Ground Color", new_color1, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
        {
            solid_color = glm::vec3(new_color1[0], new_color1[1], new_color1[2]);
            AppState::settings.ground_options.solid_color = solid_color;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("solid", &solid))
        {
            AppState::settings.ground_options.solid = solid;
        }
        shift_right();

        float new_color2[4];
        new_color2[0] = grid_color.r;
        new_color2[1] = grid_color.g;
        new_color2[2] = grid_color.b;
        if (ImGui::ColorEdit3("Grid Color", new_color2, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
        {
            grid_color = glm::vec3(new_color2[0], new_color2[1], new_color2[2]);
            AppState::settings.ground_options.grid_color = grid_color;
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("grid", &grid))
        {
            AppState::settings.ground_options.grid = grid;
        }

        shift_right();
        if (ImGui::DragFloat("height", &height, 0.1f, -100.0f, 100.0f, "%.1f"))
        {
            AppState::settings.ground_options.height = height;
        }

    }

    void ToolBar::show_shadow_menu()
    {
        auto& settings = AppState::settings;

        ImGui::Checkbox("###Shadows", &settings.shadows_active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Shadows"))
        {
            return;
        }
        shift_right();

        ImGui::SliderInt("Cascades", &settings.num_shadow_cascades, 1, 8);

    }


    void ToolBar::show_ambient_occlusion_menu()
    {
        const char* dropdown_presets[5] = {
                "OFF", "Quality", "Balanced", "Performance", "Custom"
        };

        static const int s_max_samples = 64;

        auto& settings = AppState::settings;

        ImGui::Checkbox("###Ambient Occlusion", &settings.ssao_active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Ambient Occlusion"))
        {
            return;
        }
        ImGui::InvisibleButton("###invisible-padding", ImVec2(ImGui::GetCursorPosX() - 1, ImGui::GetStyle().FramePadding.y));
        ImGui::SameLine();

        ImVec2& padding = ImGui::GetStyle().FramePadding;
        int selected_preset = static_cast<int>(settings.ssao_mode);
        ImGui::Combo(
                "##Preset",
                &selected_preset,
                dropdown_presets,
                IM_ARRAYSIZE(dropdown_presets),
                IM_ARRAYSIZE(dropdown_presets));
        settings.ssao_mode = static_cast<SSAOMode>(selected_preset);

        // custom options when users want to tweak the values themselves
        if (settings.ssao_mode == SSAOMode::CUSTOM)
        {
            auto& actual_options = settings.ssao_custom_options;
            actual_options.active = true;
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
            ImGui::SliderInt("Samples", &actual_options.num_samples, 1, s_max_samples);
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
            ImGui::SliderFloat("Radius", &actual_options.sample_radius, 0.0f, 3.0f);
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
            ImGui::SliderFloat("Strength", &actual_options.strength, 0.0, 10.0);
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
            ImGui::SliderFloat("Bias", &actual_options.z_bias, 0.0f, 0.1f);
        }


    }

    void ToolBar::show_transparency_menu()
    {
        auto& settings = AppState::settings;

        ImGui::Checkbox("###Transparency", &settings.transparency_active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Transparency"))
        {
            return;
        }
        shift_right();

        auto transparency_mode = settings.transparency_mode;

        if (ImGui::RadioButton("Depth Peeling", transparency_mode == TransparencyMode::DEPTH_PEELING))
        {
            settings.transparency_mode = TransparencyMode::DEPTH_PEELING;
        }
        //shift_right();
        if (transparency_mode == TransparencyMode::DEPTH_PEELING)
        {
            shift_right();
            ImGui::SliderInt("DP_Passes", &settings.num_depth_peeling_passes, 0, 50);
        }
        shift_right();
        if (ImGui::RadioButton("Weighted Blended", transparency_mode == TransparencyMode::WEIGHTED_BLENDED))
        {
            settings.transparency_mode = TransparencyMode::WEIGHTED_BLENDED;
        }


    }

    void ToolBar::shift_right(int x)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)x);
    }

} // namespace volumeshOS
