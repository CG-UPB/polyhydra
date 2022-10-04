
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


        ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - ImGui::GetStyle().WindowPadding.x * 0.5f + 2.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().WindowPadding.y * 0.5f);
        show_screenshot_menu();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y * 0.5f);


        show_general_menu();
        show_selection_menu();
        show_camera_menu();
        show_sky_menu();
        show_light_menu();
        show_ground_menu();
        show_bezier_mesh_menu();

        ImGui::Separator();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y);
        ImGuiUtil::push_bold_font();
        ImGui::Text("Graphics");
        ImGui::PopFont();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetStyle().WindowPadding.y);

        show_shadow_menu();
        show_ambient_occlusion_menu();
        show_transparency_menu();
        show_post_processing_menu();

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
        bool wireframe_or_vertices = settings.rendering_mode == RenderingMode::ONLY_VERTICES || settings.rendering_mode == RenderingMode::WIREFRAME;
        if (ImGuiUtil::begin_menu_with_background("rendering modes", wireframe_or_vertices? 3 : 2))
        {
            ImGuiUtil::menu_item_filled("Lighting Model", [&]
            {
                int lighting_model = static_cast<int>(AppState::settings.use_global_pbr);
                constexpr const char* lighting_model_options[] =
                        {
                                "Phong",
                                "PBR"
                        };
                if (ImGui::Combo("##Lighting Model", &lighting_model, lighting_model_options,
                                 IM_ARRAYSIZE(lighting_model_options), IM_ARRAYSIZE(lighting_model_options)))
                {
                    bool use_pbr = static_cast<bool>(lighting_model);
                    AppState::settings.ground.use_pbr = use_pbr;
                    for (const auto mesh: volumeshOS::get_meshes())
                    {
                        mesh.set_lighting_mode(static_cast<LightingMode>(use_pbr));
                    }
                    AppState::settings.use_global_pbr = use_pbr;
                }
            });
            ImGuiUtil::menu_item_filled("Mode", [&]
            {
                int rendering_mode = static_cast<int>(AppState::settings.rendering_mode);
                constexpr const char* element_mode_types[] =
                        {
                                "Lines",
                                "Points",
                                "Flat",
                                "Phong"
                        };
                ImGui::Combo("##Manual Mode RenderingMode:", &rendering_mode, element_mode_types,
                             IM_ARRAYSIZE(element_mode_types), IM_ARRAYSIZE(element_mode_types));
                AppState::settings.rendering_mode = static_cast<RenderingMode>(rendering_mode);
            });
            if(settings.rendering_mode == RenderingMode::ONLY_VERTICES)
            {
                ImGuiUtil::menu_item_filled("Vertex Size", [&]
                {
                    ImGui::DragFloat("##Vertex Size", &settings.vertex_size, 0.01f, 0.0f, 5.0f, "%.01f");
                });
            }
            else if(settings.rendering_mode == RenderingMode::WIREFRAME)
            {
                ImGuiUtil::menu_item_filled("Wireframe Size", [&]
                {
                    ImGui::DragFloat("##Wireframe Size", &settings.wireframe_size, 0.01f, 0.0f, 5.0f, "%.01f");

                });
            }
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
            ImGuiUtil::menu_item_filled("Mode", [&]
            {
                constexpr const char* selection_modes[] =
                        {
                                "Off", "Vertices", "Edges", "Halffaces", "Cells", "All"
                        };
                auto mode = static_cast<int>(AppState::settings.selection_mode);
                ImGui::Combo(
                        "##SelectionMode",
                        &mode,
                        selection_modes,
                        IM_ARRAYSIZE(selection_modes),
                        IM_ARRAYSIZE(selection_modes)
                );
                //AppState::settings.selection_mode = static_cast<SelectionMode>(m_current_selection_mode);
                AppState::settings.selection_active = AppState::settings.selection_mode != SelectionMode::OFF;
            });
            ImGuiUtil::menu_item_filled("Select by ID", [&]
            {
                constexpr const char* element_selection_types[] =
                        {
                                "Face", "Vertex", "Edge", "Cell"
                        };
                ImGui::Combo("##SelectionType", &m_manual_selection_type, element_selection_types,
                             IM_ARRAYSIZE(element_selection_types), IM_ARRAYSIZE(element_selection_types));
            });
            ImGuiUtil::menu_item_filled("", [&]
            {
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
            ImGuiUtil::menu_item_filled("Mode", [&]
            {
                constexpr const char* camera_modes[] =
                        {
                                "ORBIT", "FLY"
                        };
                int camera_mode = static_cast<int>(m_camera->get_mode());
                if (ImGui::Combo(
                        "##CameraMode",
                        &camera_mode,
                        camera_modes,
                        IM_ARRAYSIZE(camera_modes),
                        IM_ARRAYSIZE(camera_modes)
                ))
                {
                    glm::vec3 new_target = m_camera->target;
                    if (auto mesh = volumeshOS::get_focused_mesh().is_valid())
                    {
                        new_target = volumeshOS::get_focused_mesh().get_position<glm::vec3>();
                    }

                    if(m_camera->get_mode() == CameraMode::ORBIT)
                    {
                        m_camera->set_mode(CameraMode::FLY);
                    }
                    else if(m_camera->get_mode() == CameraMode::FLY)
                    {
                        m_camera->animated_look_at(new_target);
                        m_camera->set_mode(CameraMode::ORBIT);
                    }
                }
            });
            ImGuiUtil::menu_item_filled("Position", [&]
            {
                auto camera_position = m_camera->position;
                auto camera_target = m_camera->target;
                auto camera_dir = (camera_target - camera_position);
                float position[4];
                position[0] = camera_position.r;
                position[1] = camera_position.g;
                position[2] = camera_position.b;
                if (ImGui::DragFloat3("##CameraPosition", position, 0.1f, -100.0f, 100.0f, "%.1f"))
                {
                    auto pos = glm::vec3(position[0], position[1], position[2]);
                    if (m_camera->get_mode() == CameraMode::FLY)
                    {
                        camera_target = pos + camera_dir;
                        m_camera->look_at(camera_target);
                    }
                    camera_position = pos;
                    m_camera->set_position(camera_position);

                }
            });
            ImGuiUtil::menu_item_filled("FOV", [&]
            {
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
            ImGuiUtil::menu_item_filled("Direction", [&]
            {
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
            ImGuiUtil::menu_item_filled("Color", [&]
            {
                auto& light_color = settings.light.color;
                float new_color1[4];
                new_color1[0] = light_color.r;
                new_color1[1] = light_color.g;
                new_color1[2] = light_color.b;
                if (ImGui::ColorEdit3("Light Color", new_color1,
                                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    light_color = glm::vec3(new_color1[0], new_color1[1], new_color1[2]);
                }
            });
            ImGuiUtil::menu_item_filled("Size", [&]
            {
                float size = settings.light.size;
                if (ImGui::DragFloat("##LightSize", &size, 0.1f, 0.0f, 100.0f))
                {
                    settings.light.size = size;
                }
            });

            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_post_processing_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);
        ImGui::Checkbox("###Post Processing", &settings.post_processing.active);
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Post Processing"))
        {
            return;
        }
        shift_right(46 - ImGui::GetStyle().FramePadding.x);
        if (ImGuiUtil::begin_menu_with_background("post", 3))
        {
            ImGuiUtil::menu_item_filled("Gamma", [&]
            {
                ImGui::DragFloat("##Gamma", &settings.post_processing.gamma, 0.1f, 1.0f, 4.0f, "%.1f");
            });
            ImGuiUtil::menu_item_filled("Saturation", [&]
            {
                ImGui::DragFloat("##Saturation", &settings.post_processing.saturation, 0.01f, 0.0f, 2.0f, "%.2f");
            });
            ImGuiUtil::menu_item_filled("Contrast", [&]
            {
                ImGui::DragFloat("##Contrast", &settings.post_processing.contrast, 0.01f, 0.0f, 2.0f, "%.2f");
            });
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_bezier_mesh_menu()
    {
        auto& settings = AppState::settings;
        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("Bézier Meshes"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("bezier_mesh", 1))
        {
            ImGuiUtil::menu_item_filled("Tessellation Level", [&]
            {
                int tess_lvl = settings.bezier_meshes.tessellation_level;
                int max_tess_lvl;
                glGetIntegerv(GL_MAX_TESS_GEN_LEVEL,&max_tess_lvl);
                if (ImGui::DragInt("##CameraFOV", &tess_lvl, 1, 1, max_tess_lvl))
                {
                    settings.bezier_meshes.tessellation_level = tess_lvl;
                }
            });
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_sky_menu()
    {
        auto& settings = AppState::settings;
        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("Sky"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("sky", 3))
        {
            ImGuiUtil::menu_item_filled("Sky Color", [&]
            {
                auto& bg_color = settings.sky.sky_color;
                float new_bg_color[3];
                new_bg_color[0] = bg_color.r;
                new_bg_color[1] = bg_color.g;
                new_bg_color[2] = bg_color.b;
                if (ImGui::ColorEdit3("##Sky Color", new_bg_color,
                                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    bg_color = glm::vec3(new_bg_color[0], new_bg_color[1], new_bg_color[2]);
                }
            });
            ImGuiUtil::menu_item_filled("Fog Density", [&]
            {
                ImGui::SliderFloat("##Fog Density", &AppState::settings.sky.fog_density, 0.0f, 1.0f, "%.2f",
                                   ImGuiSliderFlags_Logarithmic);
            });
            ImGuiUtil::menu_item_filled("Fog Color", [&]
            {
                auto& color = AppState::settings.sky.fog_color;
                float new_color[3];
                new_color[0] = color.r;
                new_color[1] = color.g;
                new_color[2] = color.b;
                if (ImGui::ColorEdit3("##Fog Color", new_color,
                                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                {
                    color = glm::vec3(new_color[0], new_color[1], new_color[2]);
                }
            });
            ImGuiUtil::end_menu();
        }
    }


    void ToolBar::show_ground_menu()
    {
        auto& settings = AppState::settings;
        auto x = ImGui::GetCursorScreenPos().x;
        if (!ImGui::CollapsingHeader("Ground"))
        {
            return;
        }
        ImGui::SetCursorScreenPos({x - ImGui::GetStyle().FramePadding.x + 1, ImGui::GetCursorScreenPos().y});
        if (ImGuiUtil::begin_menu_with_background("ground", AppState::settings.ground.use_pbr ? 6 : 4))
        {
            ImGuiUtil::menu_item_filled("Ground Color", [&]
            {
                auto& solid_color = AppState::settings.ground.solid_color;
                float new_color[3];
                new_color[0] = solid_color.r;
                new_color[1] = solid_color.g;
                new_color[2] = solid_color.b;
                if (ImGui::ColorEdit3("##Ground Color", new_color,
                                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
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
            ImGuiUtil::menu_item_filled("Grid Color", [&]
            {
                auto& grid_color = AppState::settings.ground.grid_color;
                float new_color[3];
                new_color[0] = grid_color.r;
                new_color[1] = grid_color.g;
                new_color[2] = grid_color.b;
                if (ImGui::ColorEdit3("##Grid Color", new_color,
                                      ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
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
            ImGuiUtil::menu_item_filled("Height", [&]
            {
                float height = AppState::settings.ground.height;
                if (ImGui::DragFloat("##height", &height, 0.1f, -100.0f, 100.0f, "%.1f"))
                {
                    AppState::settings.ground.height = height;
                }
            });
            ImGuiUtil::menu_item_filled("Use PBR", [&]
            {
                constexpr const char* lighting_options[] = {
                        "Phong",
                        "PBR"
                };
                int lighting_model = static_cast<int>(AppState::settings.ground.use_pbr);
                ImGui::Combo("##Manual Mode SelectionMode:", &lighting_model, lighting_options,
                             IM_ARRAYSIZE(lighting_options), IM_ARRAYSIZE(lighting_options));
                AppState::settings.ground.use_pbr = static_cast<bool>(lighting_model);
            });
            if (AppState::settings.ground.use_pbr)
            {
                ImGuiUtil::menu_item_filled("Metallic", [&]
                {
                    float metallic = AppState::settings.ground.metallic;
                    if (ImGui::SliderFloat("##Metallic", &metallic, 0.04f, 1.0f))
                    {
                        AppState::settings.ground.metallic = metallic;
                    }
                });
                ImGuiUtil::menu_item_filled("Roughness", [&]
                {
                    float roughness = AppState::settings.ground.roughness;
                    if (ImGui::SliderFloat("##Roughness", &roughness, 0.0f, 1.0f))
                    {
                        AppState::settings.ground.roughness = roughness;
                    }
                });
            }
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_shadow_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);

        auto shadows_active = settings.shadows_active;
        if(ImGui::Checkbox("###Shadows", &shadows_active))
        {
            settings.shadows_active = shadows_active;
        }

        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Shadows"))
        {
            return;
        }
        shift_right(46 - ImGui::GetStyle().FramePadding.x);
        if (ImGuiUtil::begin_menu_with_background("shadows", 1))
        {
            ImGuiUtil::menu_item_filled("Cascades", [&]
            {
                ImGui::SliderInt("##Cascades", &settings.num_shadow_cascades, 1, 8);
            });

            ImGuiUtil::menu_item_filled("Strength", [&]
            {
                float shadow_strength = settings.shadow_strength;
                if(ImGui::SliderFloat("##Strength", &shadow_strength, 0.0f, 1.0f))
                {
                    settings.shadow_strength = shadow_strength;
                };
            });

            ImGuiUtil::end_menu();
        }
    }


    void ToolBar::show_ambient_occlusion_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);

        auto ssao_active = settings.ssao_active;
        if(ImGui::Checkbox("###SSAO", &ssao_active))
        {
            settings.ssao_active = ssao_active;
        }

        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Ambient Occlusion"))
        {
            return;
        }
        shift_right(46 - ImGui::GetStyle().FramePadding.x);
        if (ImGuiUtil::begin_menu_with_background("ssao", settings.ssao_mode == SSAOMode::CUSTOM ? 6 : 1))
        {
            ImGuiUtil::menu_item_filled("Preset", [&]
            {
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
                ImGuiUtil::menu_item_filled("Samples", [&]
                {
                    static const int s_max_samples = 64;
                    ImGui::SliderInt("##Samples", &options.num_samples, 1, s_max_samples);
                });
                ImGuiUtil::menu_item_filled("Radius", [&]
                {
                    ImGui::SliderFloat("##Radius", &options.sample_radius, 0.0f, 3.0f);
                });
                ImGuiUtil::menu_item_filled("Strength", [&]
                {
                    ImGui::SliderFloat("##Strength", &options.strength, 0.0, 10.0);
                });
                ImGuiUtil::menu_item_filled("Bias", [&]
                {
                    ImGui::SliderFloat("##Bias", &options.z_bias, 0.0f, 0.1f);
                });
                ImGuiUtil::menu_item_filled("Blur Sharpness", [&]
                {
                    ImGui::SliderFloat("##Blur Sharpness", &options.blur_sharpness, 0.0f, 100.0f, "%.1f",
                                       ImGuiSliderFlags_Logarithmic);
                });
            }
            ImGuiUtil::end_menu();
        }
    }

    void ToolBar::show_transparency_menu()
    {
        auto& settings = AppState::settings;
        shift_right(-ImGui::GetStyle().FramePadding.x);
        auto transparency_active = settings.transparency_active;
        if(ImGui::Checkbox("###Transparency", &transparency_active))
        {
            settings.transparency_active = transparency_active;
        }
        ImGui::SameLine();
        if (!ImGui::CollapsingHeader("Transparency"))
        {
            return;
        }
        shift_right(46 - ImGui::GetStyle().FramePadding.x);
        auto transparency_mode = settings.transparency_mode;
        if (ImGuiUtil::begin_menu_with_background("transparency",
                                                  transparency_mode == TransparencyMode::DEPTH_PEELING ? 3 : 2))
        {
            ImGuiUtil::menu_item_filled("Depth Peeling", [&]
            {
                if (ImGui::RadioButton("##Depth Peeling", transparency_mode == TransparencyMode::DEPTH_PEELING))
                {
                    settings.transparency_mode = TransparencyMode::DEPTH_PEELING;
                }
            });
            if (transparency_mode == TransparencyMode::DEPTH_PEELING)
            {
                ImGuiUtil::menu_item_filled("Passes", [&]
                {
                    ImGui::SliderInt("##Passes", &settings.num_depth_peeling_passes, 0, 50);
                });
            }
            ImGuiUtil::menu_item_filled("Weighted Blended", [&]
            {
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
