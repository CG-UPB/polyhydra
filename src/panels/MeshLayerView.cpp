
#include "MeshLayerView.h"
#include "../util/Tooltips.h"
#include "NewFileDialog.h"
#include "../util/ImGuiUtil.h"
#include "imgui.h"
#include "volumeshOS.h"

namespace volumeshOS::Internal
{
    void MeshLayerView::show()
    {
        if (!ImGui::Begin("Meshes"))
        {
            ImGui::End();
            return;
        }
        ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));

        // Iterate backwards, so the last added meshes are at the bottom
        const auto meshes = volumeshOS::get_meshes();
        for(auto mesh = meshes.rbegin(); mesh != meshes.rend(); mesh++)
        {
            if (mesh->is_valid())
            {
                ImGui::PushID(mesh->get_id());
                render_mesh_settings(*mesh);
                ImGui::PopID();
            }
        }
        ImGui::PopStyleColor();
        ImGui::End();
    }

    void MeshLayerView::render_mesh_options(const VMesh& mesh)
    {
        auto& icon_ref = *UIUtil::get_icon("reset.png");
        float aspect_ratio = (float) icon_ref.get_width() / (float) icon_ref.get_height();
        float width = ImGui::GetFontSize() * aspect_ratio + 2 * ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().WindowPadding.x;

        ImGuiUtil::add_background_rect(13 );
        ImGui::BeginGroup();
        if (ImGui::BeginTable("mesh", 3, ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("One", ImGuiTableColumnFlags_WidthStretch, 0.1f);
            ImGui::TableSetupColumn("Two", ImGuiTableColumnFlags_WidthStretch, 0.3f);
            ImGui::TableSetupColumn("Three", ImGuiTableColumnFlags_WidthStretch, 0.6f);

            // Mesh transformations, such as position and scale
            auto pos = mesh.get_position<glm::vec3>();
            auto scl = mesh.get_scale();
            auto rot = mesh.get_rotation<glm::vec3>();
            m_mesh_position[0] = pos[0];
            m_mesh_position[1] = pos[1];
            m_mesh_position[2] = pos[2];
            m_mesh_scale = scl;

            ImGui::BeginDisabled(!mesh.get_visibility());

            ImGuiUtil::menu_item("Rendering Mode", "icon_eye.png", width, [&]
            {
//                int rendering_mode = static_cast<int>(mesh.get_rendering_mode());
//                constexpr const char* element_mode_types[] = { "Points", "Lines", "Cells" };
//                ImGui::Combo("##RenderingMode:", &rendering_mode, element_mode_types,
//                             IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
//                mesh.set_rendering_mode(static_cast<RenderingMode>(rendering_mode));
                //ImGui::SameLine();
                bool cells = volumeshOS::is_rendering_cells(mesh);
                if(ImGui::Checkbox("  Cells", &cells))
                {
                    volumeshOS::render_cells(mesh, cells);
                }
//                ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::GetFontSize() - 2 * ImGui::GetStyle().FramePadding.x - 6.0f);
//                if(ImGuiUtil::icon_button("icon_gear.png"))
//                {
//                    ImGui::OpenPopup("Settings for Cells");
//                    ImGui::SameLine();
//                }
//                render_cells_popup(mesh);
            });

            ImGuiUtil::menu_item("", width, [&]
            {
                bool lines = volumeshOS::is_rendering_lines(mesh);
                if(ImGui::Checkbox("  Lines", &lines))
                {
                    volumeshOS::render_lines(mesh, lines);
                }
                ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::GetFontSize() - 2 * ImGui::GetStyle().FramePadding.x - 6.0f);
                if(ImGuiUtil::icon_button("icon_gear.png"))
                {
                    ImGui::OpenPopup("Settings for Lines");
                    ImGui::SameLine();
                }
                render_lines_popup(mesh);
            });

            ImGuiUtil::menu_item(" ", width, [&]
            {
                bool points = volumeshOS::is_rendering_points(mesh);
                if(ImGui::Checkbox("  Points", &points))
                {
                    volumeshOS::render_points(mesh, points);
                }
                ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::GetFontSize() - 2 * ImGui::GetStyle().FramePadding.x - 6.0f);
                if(ImGuiUtil::icon_button("icon_gear.png"))
                {
                    ImGui::OpenPopup("Settings for Points");
                    ImGui::SameLine();
                }
                render_points_popup(mesh);
            });

            ImGuiUtil::menu_item("Shading Mode", "icon_eye.png", width, [&]
            {
                int shading_mode = static_cast<int>(mesh.get_shading_mode());
                constexpr const char* element_mode_types[] = { "Flat", "Phong" };
                ImGui::Combo("##ShadingMode:", &shading_mode, element_mode_types,
                             IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
                mesh.set_shading_mode(static_cast<ShadingMode>(shading_mode));
            });
            ImGuiUtil::menu_item("Position", "icon_move.png", width, [&]
            {
                if (ImGui::DragFloat3("##Position", m_mesh_position, 0.1f, -100.0f, 100.0f, "%.1f"))
                {
                    mesh.set_position(m_mesh_position[0], m_mesh_position[1], m_mesh_position[2]);
                }
                ImGui::SameLine();
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    mesh.set_position(0.0f, 0.0f, 0.0f);
                }
            });
            ImGuiUtil::menu_item("Scale", "icon_scale.png", width, [&]
            {
                if (ImGui::DragFloat("##Scale", &m_mesh_scale, 0.01f, 0.0f, 10.0f, "%.2f"))
                {
                    mesh.set_scale(m_mesh_scale);
                }
                ImGui::SameLine();
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    mesh.set_scale(1.0f);
                }
            });

            ImGuiUtil::menu_item("Rotation", "icon_rotate.png", width, [&]
            {
                    // update GUI when mesh was rotation by anything else than the GUI
                    if(AppState::settings.rotated)
                    {
                        auto euler_angles = mesh.get_rotation<glm::vec3>();
                        m_mesh_rotation[0] = glm::degrees(euler_angles[0]);
                        m_mesh_rotation[1] = glm::degrees(euler_angles[1]);
                        m_mesh_rotation[2] = glm::degrees(euler_angles[2]);

                        AppState::settings.rotated = false;
                    }

                    float angles[3] = {m_mesh_rotation[0], m_mesh_rotation[1], m_mesh_rotation[2]};
                    if (ImGui::DragFloat3("##Rotation", angles, 1.0f, -360.0f, 360.0f, "%.1f"))
                    {

                        auto delta_x = angles[0] - m_mesh_rotation[0];
                        auto delta_y = angles[1] - m_mesh_rotation[1];
                        auto delta_z = angles[2] - m_mesh_rotation[2];

                        m_mesh_rotation[0] = angles[0];
                        m_mesh_rotation[1] = angles[1];
                        m_mesh_rotation[2] = angles[2];


                        mesh.set_rotation(delta_x, delta_y, delta_z);

                    }
                    ImGui::SameLine();
                    if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                    {
                        m_mesh_rotation[0] = 0.0f;
                        m_mesh_rotation[1] = 0.0f;
                        m_mesh_rotation[2] = 0.0f;
                        mesh.reset_rotation();
                    }

            });

            ImGuiUtil::menu_item("Slicer", "icon_slice.png", width, [&]
            {
                m_slider_slicer = mesh.get_slice_factor();
                m_slicer_locked = mesh.get_slice_lock();
                if (ImGui::SliderFloat("##Slice", &m_slider_slicer, 0.0f, 1.0f))
                {
                    mesh.set_slice_factor(m_slider_slicer);
                }
                ImGui::SameLine();
                if (ImGuiUtil::icon_button(m_slicer_locked ? "icon_locked.png" : "icon_unlocked.png", ImGui::GetFontSize()))
                {
                    m_slicer_locked = !m_slicer_locked;
                    mesh.set_slice_locked(m_slicer_locked);
                }
            });
            ImGuiUtil::menu_item("Peel", "icon_peel.png", width, [&]
            {

                m_slider_peel = mesh.get_peel_level();
                float peel_max = (float) mesh.get_max_peel_depth() + 1.0f;

                // make it easier to get the slider onto an Integer
                // thats helpful for peeling with transparent transition
                static float tolerance = 0.05;

                if (ImGui::SliderFloat("##Peel", &m_slider_peel, 0, peel_max))
                {
                    if ((int) (m_slider_peel + tolerance) != (int) (m_slider_peel - tolerance))
                    {
                        m_slider_peel = (float) (int) (m_slider_peel + tolerance);
                    }
                    mesh.set_peel_level(m_slider_peel);
                }
                ImGui::SameLine();
                auto reverse_peeling = mesh.is_using_reverse_peeling();
                if (ImGuiUtil::icon_button(reverse_peeling ? "icon_peel_inner.png" : "icon_peel_outer.png", ImGui::GetFontSize()))
                {
                    mesh.use_reverse_peeling(!reverse_peeling);
                }
            });
            ImGuiUtil::menu_item("Cell Size", "icon_cell_size.png", width, [&]
            {
                m_cell_size = mesh.get_cell_size();
                if (ImGui::SliderFloat("##CellSize", &m_cell_size, 0.0f, 1.0f))
                {
                    mesh.set_cell_size(m_cell_size);
                }
            });
            if(mesh.is_bezier_mesh())
            {
                ImGuiUtil::menu_item("Tessellation", "icon_roundings.png", width, [&]
                {
                    int tesselation_level = mesh.get_tessellation_level();
                    if (ImGui::SliderInt("", &tesselation_level, 1, 64))
                    {
                        mesh.set_tessellation_level(tesselation_level);
                    }
                });
            }
            else
            {
                ImGuiUtil::menu_item("Roundings", "icon_roundings.png", width, [&]
                {
                    float actual_rounding_size = mesh.get_cell_rounding();
                    if (ImGui::SliderFloat("", &actual_rounding_size, 0.0f, 1.0f, "%.3f"))
                    {
                        mesh.set_cell_rounding(actual_rounding_size);
                    }
                });
            }
            ImGuiUtil::menu_item("Digging", "icon_dig.png", width, [&]
            {
                auto size = ImVec2(ImGui::CalcTextSize("Deactivate").x + 2 * ImGui::GetStyle().FramePadding.x,
                                   0.0f);
                if (ImGui::Button(m_digging_activated ? "Deactivate" : "Activate", size))
                {
                    if (!m_digging_activated)
                    {
                        m_digging_activated = true;
                        AppState::settings.selection_active = true;
                        AppState::settings.selection_mode = SelectionMode::CELL;
                    }
                    else
                    {
                        m_digging_activated = false;
                    }
                    AppState::settings.digging_active = m_digging_activated;
                }
                ImGui::SameLine();
                if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                {
                    mesh.reset_visibility();
                }
            });
            ImGuiUtil::menu_item_filled("Isolation", "icon_isolate.png", [&]
            {
                auto size = ImVec2(ImGui::CalcTextSize("Deactivate").x + 2 * ImGui::GetStyle().FramePadding.x,
                                   0.0f);
                if (ImGui::Button(m_isolation_started ? "Deactivate" : "Activate", size))
                {
                    if (!m_isolation_started)
                    {
                        m_isolation_started = true;
                        AppState::settings.selection_active = true;
                        AppState::settings.selection_mode = SelectionMode::CELL;
                    }
                    else
                    {
                        m_isolation_started = false;
                        mesh.reset_visibility();
                    }
                    AppState::settings.isolation_active = m_isolation_started;
                }
            });
            ImGui::EndDisabled();
            ImGuiUtil::end_menu();
        }
    }

    void MeshLayerView::render_mesh_settings(const VMesh& mesh)
    {
        // Visibility
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().FramePadding.x);
            auto mesh_visible = mesh.get_visibility();
            if (ImGui::Checkbox("###Visible", &mesh_visible))
            {
                mesh.set_visibility(mesh_visible);
            }
            ImGui::SameLine();
        }
        ImGui::BeginDisabled(!mesh.get_visibility());
        // Color
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().FramePadding.x);
            auto mesh_color = mesh.get_color<glm::vec4>();
            float new_color[4] = { mesh_color.r, mesh_color.g, mesh_color.b, mesh_color.a };
            if (ImGui::ColorEdit4("Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
            {
                mesh.set_color(glm::vec4{ new_color[0], new_color[1], new_color[2], new_color[3] });
            }
            ImGui::SameLine();
        }
        // Settings
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().FramePadding.x);
            if (ImGuiUtil::icon_button("icon_gear.png"))
            {
                ImGui::OpenPopup("Advanced Settings Popup");
            }
            ImGui::SameLine();
        }
        render_popup(mesh);
        if(!mesh.get_visibility())
        {
            ImGui::SetNextTreeNodeOpen(false);
        }
        if (!render_header(mesh))
        {
            ImGui::EndDisabled();
            return;
        }
        ImGui::InvisibleButton("invisible button shadow", ImVec2(2, 2));
        ImGui::SetCursorPosX(ImGui::GetCursorPos().x - ImGui::GetStyle().FramePadding.x);
        ImGui::EndDisabled();
        render_mesh_options(mesh);

        // some space to the next item
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 2));
    }

    void MeshLayerView::render_popup(const VMesh& mesh)
    {
        ImGui::SetNextWindowSize({400.0f, 0.0f});
        if (ImGui::BeginPopup("Advanced Settings Popup"))
        {
            ImGuiUtil::push_bold_font();
            ImGui::Text("Configuration");
            ImGui::PopFont();
            ImGuiUtil::add_padding_y(0.5f);

            ImGuiUtil::icon("save.png", ImGui::GetFontSize(), true);
            ImGui::SameLine();
            if (ImGui::Button("Save"))
            {
                NewFileDialog file_dialog;

                char const* filename;

                filename = file_dialog.save_mesh_settings("Save Mesh Settings File");

                if (filename != nullptr)
                {
                    mesh.save_configuration(filename);
                }
            }
            ImGui::SameLine();
            // Load Mesh Settings to File
            if (ImGui::Button("Load"))
            {
                NewFileDialog file_dialog;

                char const* filename;

                filename = file_dialog.load_mesh_settings("Save Mesh Settings File");
                if (filename != nullptr)
                {
                    mesh.load_configuration(filename);
                }
            }

            ImGuiUtil::add_padding_y(0.5f);
            ImGui::Separator();
            ImGuiUtil::add_padding_y(0.5f);
            ImGuiUtil::push_bold_font();
            ImGui::Text("Lighting");
            ImGui::PopFont();

            auto base_color = mesh.is_using_base_color();
            if(ImGui::Checkbox("Base Color", &base_color))
            {
                use_base_color(mesh, base_color);
            }

            auto culling = mesh.is_using_backface_culling();
            if(ImGui::Checkbox("Backface Culling", &culling))
            {
                use_backface_culling(mesh, culling);
            }

            auto ts_lighting = mesh.is_using_two_sided_lighting();
            if(ImGui::Checkbox("Two-Sided Lighting", &ts_lighting))
            {
                use_two_sided_lighting(mesh, ts_lighting);
            }


            ImGuiUtil::add_padding_y(0.5f);
            ImGui::Separator();
            ImGuiUtil::add_padding_y(0.5f);
            ImGuiUtil::push_bold_font();
            ImGui::Text("Material");
            ImGui::PopFont();

            float slider_width = 200.0f;
            float padding_right = 20.0f;

            ImGui::SetNextItemWidth(slider_width);
            ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);

            constexpr const char* lighting_options[] = { "Phong", "PBR" };
            int lighting_model = static_cast<int>(mesh.get_lighting_mode());
            ImGui::Combo("##Manual Mode SelectionMode:", &lighting_model, lighting_options,
                         IM_ARRAYSIZE(lighting_options), IM_ARRAYSIZE(lighting_options));
            auto mode = static_cast<LightingMode>(lighting_model);
            mesh.set_lighting_mode(mode);

            ImGuiUtil::add_padding_y(0.5f);

            if (mode == LightingMode::PBR)
            {
                ImGui::Text("Metallic:");
                float metallic_value = mesh.get_metallic();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##Metallic", &metallic_value, 0.04f, 1.0f))
                {
                    mesh.set_metallic(metallic_value);
                }

                ImGui::Text("Roughness:");
                float roughness_value = mesh.get_roughness();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##Roughness", &roughness_value, 0.04f, 1.0f))
                {
                    mesh.set_roughness(roughness_value);
                }
            }
            else
            {
                // Ambient
                ImGui::Text("Ambient:");
                float ambient_value = mesh.get_ambient();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##Ambient", &ambient_value, 0.0f, 1.0f))
                {
                    mesh.set_ambient(ambient_value);
                }

                // Diffuse
                ImGui::Text("Diffuse:");
                float diffuse_value = mesh.get_diffuse();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##Diffuse", &diffuse_value, 0.0f, 1.0f))
                {
                    mesh.set_diffuse(diffuse_value);
                }

                // Specular
                ImGui::Text("Specular:");
                float specular_value = mesh.get_specular();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##Specular", &specular_value, 0.0f, 1.0f))
                {
                    mesh.set_specular(specular_value);
                }

                // Specular Exponent
                ImGui::Text("Specular Exponent:");
                float specular_exp = mesh.get_specular_coefficient();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                if (ImGui::SliderFloat("##Specular Exponent", &specular_exp, 0.0f, 10.0f))
                {
                    mesh.set_specular_coefficient(specular_exp);
                }
            }
            ImGui::EndPopup();
        }
    }

    bool MeshLayerView::render_header(const VMesh& mesh)
    {
        bool open = true;
        auto focused = mesh.is_focused();
        if (focused)
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Button));
        }
        auto flags = ImGuiTreeNodeFlags_AllowItemOverlap;
        if (!ImGui::CollapsingHeader(mesh.get_name().c_str(), flags))
        {
            open = false;
        }
        if (focused)
        {
            ImGui::PopStyleColor();
        }
        ImGui::SameLine(ImGui::GetContentRegionAvailWidth());
        if (ImGui::Checkbox("###Focused", &focused))
        {
            if (focused)
            {
                mesh.set_focused();
            }
        }
        return open;
    }

    void MeshLayerView::render_cells_popup(const VMesh &mesh)
    {
        if (ImGui::BeginPopup("Settings for Cells"))
        {
            ImGui::Text("Cells Settings");
            ImGui::Separator();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            float new_color[4] = {0.5, 0.5, 0.5, 1.0};
            if (ImGui::ColorEdit4("Cell Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
            {

            }
            ImGui::EndPopup();
        }
    }

    void MeshLayerView::render_lines_popup(const VMesh &mesh)
    {
        if (ImGui::BeginPopup("Settings for Lines"))
        {
            ImGui::Text("Lines Settings");
            ImGui::Separator();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            float line_width = mesh.get_line_width();
            ImGui::SliderFloat("##Line Width", &line_width, 0.0f, 10.0f, "%.01f");
            mesh.set_line_width(line_width);

            ImGui::SameLine();

            auto point_color = mesh.get_line_color<glm::vec3>();
            float new_color[3] = { point_color.r, point_color.g, point_color.b};
            if (ImGui::ColorEdit3("Line Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
            {
                mesh.set_line_color(glm::vec3{ new_color[0], new_color[1], new_color[2]});
            }

            ImGui::EndPopup();
        }
    }

    void MeshLayerView::render_points_popup(const VMesh &mesh)
    {
        if (ImGui::BeginPopup("Settings for Points"))
        {
            ImGui::Text("Points Settings");
            ImGui::Separator();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);

            float point_size = mesh.get_point_size();
            ImGui::SliderFloat("##Point Size", &point_size, 0.0f, 10.0f, "%.01f");
            mesh.set_point_size(point_size);

            ImGui::SameLine();

            auto line_color = mesh.get_point_color<glm::vec3>();
            float new_color[3] = { line_color.r, line_color.g, line_color.b};

            if (ImGui::ColorEdit3("Point Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
            {
                mesh.set_point_color(glm::vec3{ new_color[0], new_color[1], new_color[2]});
            }

            ImGui::EndPopup();
        }
    }


} // namespace volumeshOS