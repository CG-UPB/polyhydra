
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

        // create a line for every loaded mesh
        auto active_mesh = volumeshOS::get_focused_mesh();
        int active_mesh_id = active_mesh.get_id();
        for (const auto& mesh: volumeshOS::get_meshes())
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().FramePadding.x);

            int id = mesh.get_id();
            ImGui::PushID(id);
            if (ImGui::RadioButton("", &active_mesh_id, id))
            {
                volumeshOS::set_focused_mesh(VMesh(active_mesh_id));
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                volumeshOS::focus_camera_on_mesh(VMesh(active_mesh_id));
            }

            Tooltips::ToolTipByHovering("These Radio Buttons show which Mesh is active.");

            ImGui::SameLine();

            auto pre_cursor = ImGui::GetCursorPos();

            // render the actual mesh settings
            render_mesh_setting(mesh);

            auto post_cursor = ImGui::GetCursorPos();

            ImGui::SetCursorPos(pre_cursor);
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetFontSize()));
            auto size = 3.0f * ImGui::GetFrameHeight() + 2.0f * ImGui::GetStyle().FramePadding.x - 1.0f;
            ImGui::SameLine(ImGui::GetContentRegionMax().x - size);

            bool visible = mesh.get_visibility();
            if (ImGui::Checkbox("##Visible", &visible))
            {
                mesh.set_visibility(visible);
            }
            ImGui::SameLine();
            Tooltips::ToolTipByHovering("If the Checkbox is clicked, the mesh is visible");


            auto color = mesh.get_color<glm::vec4>();
            float new_color[4];
            new_color[0] = color.r;
            new_color[1] = color.g;
            new_color[2] = color.b;
            new_color[3] = color.a;
            ImGui::ColorEdit4("Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            mesh.set_color(glm::vec4{new_color[0], new_color[1], new_color[2], new_color[3]});
            Tooltips::ToolTipByHovering("Sets the color of the mesh");

            // Advanced Settings

            auto& icon_ref = *UIUtil::get_icon("icon_gear.png");
            float aspect_ratio = (float) icon_ref.get_width() / (float) icon_ref.get_height();
            float width = (ImGui::GetFontSize() + 2 * ImGui::GetStyle().FramePadding.y ) * aspect_ratio;
            float height = ImGui::GetFontSize() + 2 * ImGui::GetStyle().FramePadding.y ;
            ImVec4 text_color = ImGui::GetStyleColorVec4(ImGuiCol_Text);

            auto cursor = ImGui::GetCursorPos();
            ImGui::SetCursorPos(ImVec2(cursor.x , cursor.y ));
            if (ImGui::ImageButton(
                    reinterpret_cast<ImTextureID>(icon_ref.get_id()),
                    {width, height},
                    {0.0f, 0.0f},
                    {1.0f, 1.0f},
                    0,
                    {0.0f, 0.0f, 0.0f, 0.0f},
                    text_color
            ))
            {
                ImGui::OpenPopup("Advanced Settings Popup");
            }

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

                constexpr const char* lighting_options[] = {
                        "Phong",
                        "PBR"
                };
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
            ImGui::PopID();

            ImGui::SetCursorPos(post_cursor);

        }
        ImGui::PopStyleColor();
        ImGui::End();
    }

    void MeshLayerView::render_mesh_setting(const VMesh& mesh)
    {
        // If there is at least one mesh, the Active Mesh Settings (Slicing, Peeling, etc.) are available
        if (mesh.is_valid())
        {

            auto cursor_pos = ImGui::GetCursorPos();

            if (ImGui::CollapsingHeader(mesh.get_name().c_str(), ImGuiTreeNodeFlags_AllowItemOverlap))
            {
                auto& icon_ref = *UIUtil::get_icon("reset.png");
                float aspect_ratio = (float) icon_ref.get_width() / (float) icon_ref.get_height();
                float width = ImGui::GetFontSize() * aspect_ratio + 2 * ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().WindowPadding.x;
                //width = ImGui::GetContentRegionAvailWidth() - width - 2 * ImGui::GetStyle().FramePadding.x;


                ImGui::SetCursorPos({cursor_pos.x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorPos().y});
                ImGuiUtil::add_background_rect(9);
                ImGui::BeginGroup();
                if (ImGui::BeginTable("mesh", 3, ImGuiTableFlags_SizingFixedFit))
                {
                    ImGui::TableSetupColumn("One", ImGuiTableColumnFlags_WidthStretch, 0.1f);
                    ImGui::TableSetupColumn("Two", ImGuiTableColumnFlags_WidthStretch, 0.3f);
                    ImGui::TableSetupColumn("Three", ImGuiTableColumnFlags_WidthStretch, 0.6f);
                    //ImGui::TableHeadersRow();


                    // Mesh transformations, such as position and scale
                    auto pos = mesh.get_position<glm::vec3>();
                    auto scl = mesh.get_scale();
                    auto rot = mesh.get_rotation<glm::vec3>();
                    m_mesh_position[0] = pos[0];
                    m_mesh_position[1] = pos[1];
                    m_mesh_position[2] = pos[2];
                    m_mesh_scale = scl;
                    m_mesh_rotation[0] = glm::degrees(rot[0]);
                    m_mesh_rotation[1] = glm::degrees(rot[1]);
                    m_mesh_rotation[2] = glm::degrees(rot[2]);

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

                    //TODO Fix Object Rotation
                    ImGui::BeginDisabled(true);
                    ImGuiUtil::menu_item("Rotation", "icon_rotate.png", width, [&]
                    {
                        if (ImGui::DragFloat3("##Rotation", m_mesh_rotation, 1.0f, -180.0f, 180.0f, "%.1f"))
                        {
                            auto x = (m_mesh_rotation[0] + 180.0f) - glm::degrees(rot[0]) + 180.0f;
                            auto y = (m_mesh_rotation[1] + 180.0f) - glm::degrees(rot[1]) + 180.0f;
                            auto z = (m_mesh_rotation[2] + 180.0f) - glm::degrees(rot[2]) + 180.0f;
                            auto epsilon = 0.01;

                            if (x >= epsilon || y >= epsilon || z >= epsilon)
                            {
                                mesh.set_rotation(glm::radians(x), glm::radians(y), glm::radians(z));
                            }
                        }
                        ImGui::SameLine();
                        if (ImGuiUtil::icon_button("reset.png", ImGui::GetFontSize(), true))
                        {
                            mesh.reset_rotation();
                        }

                    });
                    ImGui::EndDisabled();


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
                            mesh.set_slice_lock(m_slicer_locked);
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
                        auto reverse_peeling = mesh.get_reverse_peeling();
                        if (ImGuiUtil::icon_button(reverse_peeling ? "icon_peel_inner.png" : "icon_peel_outer.png", ImGui::GetFontSize()))
                        {
                            mesh.set_reverse_peeling(!reverse_peeling);
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

                    ImGuiUtil::end_menu();
                }
            }
            ImGui::Separator();
        }


    }
} // namespace volumeshOS