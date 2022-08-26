
#include "ToolBar.h"
#include "NewFileDialog.h"
#include "../util/Tooltips.h"
#include "../util/ImGuiUtil.h"
#include "volumeshOS.h"
#include "../rendering/Renderer.h"

#define RECT_BG = IM_COL32(80, 80, 80, 25)

namespace volumeshOS::Internal
{

    void ToolBar::show(const std::shared_ptr<Internal::Camera>& cam)
    {
        m_camera = cam;
        show();
    }

    void ToolBar::show()
    {
        if (!ImGui::Begin("Options"))
        {
            ImGui::End();
            return;
        }

        //ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));
        ImGui::SetScrollX(1);

        ImGuiUtil::push_bold_font();
        ImGui::Text("Settings");
        ImGui::PopFont();


        ImGui::SameLine(ImGui::GetWindowWidth() - 38.0f - m_padding_right - 2.0f);
        show_screenshot_menu();

        /* ##########  TEST  ############*/

        show_general_menu();
        show_selection_menu();
        show_camera_menu();
        show_light_menu();

        ImGui::Separator();
        ImGuiUtil::push_bold_font();
        ImGui::Text("Graphics");
        ImGui::PopFont();

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
                ImGui::SetNextItemWidth(m_slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
                if (ImGui::DragFloat3("##Position", m_mesh_position, 0.1f, -100.0f, 100.0f, "%.1f"))
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
                ImGui::SetNextItemWidth(m_slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
                ImGui::SetNextItemWidth(m_slider_width - 50.0f);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
                ImGui::SetNextItemWidth(m_slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
                ImGui::SliderFloat("", &m_slider_slicer, 0.0f, 1.0f);
                ImGui::Text(" ");
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
                ImGui::SetNextItemWidth(m_slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
                if (ImGui::SliderFloat(" ", &m_slider_peel, 0, peel_max))
                {
                    active_mesh.set_peel_level(m_slider_peel);
                }

                m_cell_size = active_mesh.get_cell_size();
                ImGui::Separator();
                ImGui::Text("Cell Size");
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark("This slider will change the size of each cell");
                ImGui::SetNextItemWidth(m_slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
                ImGui::SetNextItemWidth(m_slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
                ImGui::SetNextItemWidth(m_slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
                ImGui::SetNextItemWidth(m_slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - m_slider_width - m_padding_right);
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
        //ImGui::PopStyleColor();
        ImGui::End();
    }

    void ToolBar::show_screenshot_menu()
    {
        if (ImGuiUtil::icon_button("camera.png"))
        {
            ImGui::OpenPopup("Export Settings Popup");
        }

        ImGui::SetNextWindowSize({400.0f, 0.0f});
        if (ImGui::BeginPopup("Export Settings Popup"))
        {
            m_export_dimensions[0] = m_export_options.width;
            m_export_dimensions[1] = m_export_options.height;
            if (ImGui::InputInt2("Dimensions", m_export_dimensions))
            {
                m_export_options.width = m_export_dimensions[0];
                m_export_options.height = m_export_dimensions[1];
            }
            ImGui::Checkbox("Include background", &m_export_options.include_background);
            ImGui::Checkbox("Include shapes", &m_export_options.include_shapes);
            ImGui::Checkbox("Include ground", &m_export_options.include_ground);
            ImGui::Checkbox("Ground shadow only", &m_export_options.ground_shadow_only);
            if (ImGui::Button("Export"))
            {
                NewFileDialog file_dialog;
                auto fileName = file_dialog.save_dialog("Open Snapshot File");
                if (fileName != nullptr)
                {
                    volumeshOS::export_image(fileName, m_export_options);
                }
            }
            ImGui::EndPopup();
        }
        else
        {
            m_export_options.width = volumeshOS::get_viewport_width();
            m_export_options.height = volumeshOS::get_viewport_height();
        }
    }

    void ToolBar::show_general_menu()
    {
        auto& settings = AppState::settings;
        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("General"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("rendering modes", 3))
        {
            ImGuiUtil::menu_item("Mode", [&]{
                int rendering_mode = static_cast<int>(AppState::settings.rendering_mode);
                constexpr const char* element_mode_types[] =
                        {
                                "Lines",
                                "Points",
                                "Flat",
                                "Phong"
                        };
                ImGui::Combo("##Manual Mode SelectionMode:", &rendering_mode, element_mode_types,
                             IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
                AppState::settings.rendering_mode = static_cast<RenderingMode>(rendering_mode);
            });
            ImGuiUtil::menu_item("Gamma", [&]{
                ImGui::DragFloat("##Gamma", &settings.general.gamma, 0.1f, 1.0f, 4.0f, "%.1f");
            });
            ImGuiUtil::menu_item("Background", [&]{
                auto& bg_color = settings.general.background_color;
                float new_bg_color[3];
                new_bg_color[0] = bg_color.r;
                new_bg_color[1] = bg_color.g;
                new_bg_color[2] = bg_color.b;
                if (ImGui::ColorEdit3("##Background", new_bg_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    bg_color = glm::vec3(new_bg_color[0], new_bg_color[1], new_bg_color[2]);
                }
            });
            ImGuiUtil::end_menu();
        }
    }


    void ToolBar::show_selection_menu()
    {
        auto& settings = AppState::settings;

        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("Selection"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("selection", 3))
        {
            ImGuiUtil::menu_item("Mode", [&]{
                constexpr const char* selection_modes[] =
                        {
                                "Off", "Vertices", "Edges", "Halffaces", "Cells", "All"
                        };
                ImGui::Combo(
                        "##SelectionMode",
                        &m_current_selection_mode,
                        selection_modes,
                        IM_ARRAYSIZE(selection_modes),
                        IM_ARRAYSIZE(selection_modes)
                );
                AppState::settings.selection_mode = static_cast<SelectionMode>(m_current_selection_mode);
                AppState::settings.selection_active = AppState::settings.selection_mode != SelectionMode::OFF;
            });
            ImGuiUtil::menu_item("Select by ID", [&]{
                constexpr const char* element_selection_types[] =
                        {
                                "Face", "Vertex", "Edge", "Cell"
                        };
                ImGui::Combo("##SelectionType", &m_manual_selection_type, element_selection_types,
                             IM_ARRAYSIZE(element_selection_types), IM_ARRAYSIZE(element_selection_types));
            });
            ImGuiUtil::menu_item("", [&]{
                ImGui::InputInt("##ManualSelectionID", &m_manual_selection_id);
                if (m_manual_selection_id != m_previous_manual_selection_id && m_manual_selection_id >= 0)
                {
                    m_previous_manual_selection_id = m_manual_selection_id;
                }
            });
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_camera_menu()
    {
        auto& settings = AppState::settings;
        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("Camera"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("camera", 3))
        {
            ImGuiUtil::menu_item("Mode", [&]{
                constexpr const char* camera_modes[] =
                        {
                                "ORBIT", "FLY"
                        };
                int camera_mode = static_cast<int>(m_camera->get_mode());
                if(ImGui::Combo(
                        "##CameraMode",
                        &camera_mode,
                        camera_modes,
                        IM_ARRAYSIZE(camera_modes),
                        IM_ARRAYSIZE(camera_modes)
                ))
                {
                    m_camera->set_mode(static_cast<CameraMode>(camera_mode));
                }
            });
            ImGuiUtil::menu_item("Position", [&]{
                auto& camera_position = m_camera->position;
                auto& camera_target = m_camera->target;
                auto camera_dir = (camera_target - camera_position);
                float position[4];
                position[0] = camera_position.r;
                position[1] = camera_position.g;
                position[2] = camera_position.b;
                if (ImGui::DragFloat3("##CameraPosition", position, 0.1f, -100.0f, 100.0f, "%.1f"))
                {
                    auto pos = glm::vec3(position[0], position[1], position[2]);
                    if(m_camera->get_mode() == CameraMode::FLY)
                    {
                        camera_target = pos + camera_dir;
                    }
                    camera_position = pos;

                }
            });
            ImGuiUtil::menu_item("FOV", [&]{
                float fov = m_camera->zoom;
                if (ImGui::DragFloat("##CameraFOV", &fov, 1.0f, 1.0f, 90.0f))
                {
                    m_camera->zoom = fov;
                }
            });
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_light_menu()
    {
        auto& settings = AppState::settings;
        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("Light"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("light", 2))
        {
            ImGuiUtil::menu_item("Direction", [&]{
                auto& light_direction = settings.light.direction;
                float direction[4];
                direction[0] = light_direction.r;
                direction[1] = light_direction.g;
                direction[2] = light_direction.b;
                if (ImGui::DragFloat3("##LightDirection", direction, 0.1f, -1.0f, 1.0f, "%.001f"))
                {
                    light_direction = glm::vec3(direction[0], direction[1], direction[2]);
                }
            });
            ImGuiUtil::menu_item("Color", [&]{
                auto& light_color = settings.light.color;
                float new_color1[4];
                new_color1[0] = light_color.r;
                new_color1[1] = light_color.g;
                new_color1[2] = light_color.b;
                if (ImGui::ColorEdit3("Light Color", new_color1, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    light_color = glm::vec3(new_color1[0], new_color1[1], new_color1[2]);
                }
            });
            ImGuiUtil::end_menu();
        }
    }


    void ToolBar::show_ground_menu()
    {
        auto& settings = AppState::settings;
        bool visible = settings.ground.visible;
        shift_right(-ImGui::GetStyle().FramePadding.x);
        if(ImGui::Checkbox("###Ground", &visible))
        {
            settings.ground.visible = visible;
        }
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Ground"))
        {
            return;
        }
        shift_right(30 - ImGui::GetStyle().FramePadding.x);
        if (ImGuiUtil::begin_menu_with_background("ground", 3))
        {
            ImGuiUtil::menu_item("Ground Color", [&]{
                auto& solid_color = AppState::settings.ground.solid_color;
                float new_color[3];
                new_color[0] = solid_color.r;
                new_color[1] = solid_color.g;
                new_color[2] = solid_color.b;
                if (ImGui::ColorEdit3("##Ground Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    solid_color = glm::vec3(new_color[0], new_color[1], new_color[2]);
                }
                ImGui::SameLine();
                bool solid = AppState::settings.ground.solid;
                if (ImGui::Checkbox("##solid", &solid))
                {
                    AppState::settings.ground.solid = solid;
                }
            });
            ImGuiUtil::menu_item("Grid Color", [&]{
                auto& grid_color = AppState::settings.ground.grid_color;
                float new_color[3];
                new_color[0] = grid_color.r;
                new_color[1] = grid_color.g;
                new_color[2] = grid_color.b;
                if (ImGui::ColorEdit3("##Grid Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    grid_color = glm::vec3(new_color[0], new_color[1], new_color[2]);
                }
                ImGui::SameLine();
                bool grid = AppState::settings.ground.grid;
                if (ImGui::Checkbox("##grid", &grid))
                {
                    AppState::settings.ground.grid = grid;
                }
            });
            ImGuiUtil::menu_item("Height", [&]{
                float height = AppState::settings.ground.height;
                if (ImGui::DragFloat("##height", &height, 0.1f, -100.0f, 100.0f, "%.1f"))
                {
                    AppState::settings.ground.height = height;
                }
            });
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_shadow_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);
        ImGui::Checkbox("###Shadows", &settings.shadows_active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Shadows"))
        {
            return;
        }
        shift_right(30 - ImGui::GetStyle().FramePadding.x);
        if (ImGuiUtil::begin_menu_with_background("shadows", 1))
        {
            ImGuiUtil::menu_item("Cascades", [&]{
                ImGui::SliderInt("##Cascades", &settings.num_shadow_cascades, 1, 8);
            });
            ImGuiUtil::end_menu();
        }
    }


    void ToolBar::show_ambient_occlusion_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);
        ImGui::Checkbox("###Ambient Occlusion", &settings.ssao_active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Ambient Occlusion"))
        {
            return;
        }
        shift_right(30 - ImGui::GetStyle().FramePadding.x);
        if (ImGuiUtil::begin_menu_with_background("ssao", settings.ssao_mode == SSAOMode::CUSTOM ? 6 : 1))
        {
            ImGuiUtil::menu_item("Preset", [&]{
                constexpr const char* dropdown_presets[5] = {
                        "Off", "Quality", "Balanced", "Performance", "Custom"
                };
                int selected_preset = static_cast<int>(settings.ssao_mode);
                ImGui::Combo(
                        "##Preset",
                        &selected_preset,
                        dropdown_presets,
                        IM_ARRAYSIZE(dropdown_presets),
                        IM_ARRAYSIZE(dropdown_presets));
                settings.ssao_mode = static_cast<SSAOMode>(selected_preset);
            });
            // custom options when users want to tweak the values themselves
            if (settings.ssao_mode == SSAOMode::CUSTOM)
            {
                auto& options = settings.ssao_custom;
                options.active = true;
                ImGuiUtil::menu_item("Samples", [&]{
                    static const int s_max_samples = 64;
                    ImGui::SliderInt("##Samples", &options.num_samples, 1, s_max_samples);
                });
                ImGuiUtil::menu_item("Radius", [&]{
                    ImGui::SliderFloat("##Radius", &options.sample_radius, 0.0f, 3.0f);
                });
                ImGuiUtil::menu_item("Strength", [&]{
                    ImGui::SliderFloat("##Strength", &options.strength, 0.0, 10.0);
                });
                ImGuiUtil::menu_item("Bias", [&]{
                    ImGui::SliderFloat("##Bias", &options.z_bias, 0.0f, 0.1f);
                });
                ImGuiUtil::menu_item("Distance Bias", [&]{
                    ImGui::SliderFloat("##Distance Bias", &options.distance_bias, 0.0f, 10.0f);
                });
            }
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_transparency_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);
        ImGui::Checkbox("###Transparency", &settings.transparency_active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Transparency"))
        {
            return;
        }
        shift_right(30 - ImGui::GetStyle().FramePadding.x);
        auto transparency_mode = settings.transparency_mode;
        if (ImGuiUtil::begin_menu_with_background("transparency",
                                                  transparency_mode == TransparencyMode::DEPTH_PEELING ? 3 : 2))
        {
            ImGuiUtil::menu_item("Depth Peeling", [&]{
                if (ImGui::RadioButton("##Depth Peeling", transparency_mode == TransparencyMode::DEPTH_PEELING))
                {
                    settings.transparency_mode = TransparencyMode::DEPTH_PEELING;
                }
            });
            if (transparency_mode == TransparencyMode::DEPTH_PEELING)
            {
                ImGuiUtil::menu_item("Passes", [&]{
                    ImGui::SliderInt("##Passes", &settings.num_depth_peeling_passes, 0, 50);
                });
            }
            ImGuiUtil::menu_item("Weighted Blended", [&]{
                if (ImGui::RadioButton("##Weighted Blended", transparency_mode == TransparencyMode::WEIGHTED_BLENDED))
                {
                    settings.transparency_mode = TransparencyMode::WEIGHTED_BLENDED;
                }
            });
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::shift_right(float x)
    {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)x);
    }

} // namespace volumeshOS
