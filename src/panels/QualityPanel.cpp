
#include "QualityPanel.h"
#include "../input/Input.h"
#include "../util/Tooltips.h"

namespace volumeshOS
{
    QualityPanel::QualityPanel() {
    }

    // Destruktor
    QualityPanel::~QualityPanel() {
        //delete instance;
    }


    void QualityPanel::show() {
        if (ImGui::Begin("Graphics"))
        {
            int mesh_mode = GlobalViewerSettings::getInstance()->get_mesh_mode();
            if (mesh_mode == Wireframe || mesh_mode == Only_Vertices){
                ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),"Some graphics settings are not available, \n"
                                                               "since Wireframe or Vertex-Only mode is activated.");

                ImGui::PushID("Wireframe");
                ImGui::Text("Wireframe size");
                float wireframe_size = GlobalViewerSettings::getInstance()->get_wireframe_size();
                if (ImGui::SliderFloat("##Size", &wireframe_size, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic))
                {
                    GlobalViewerSettings::getInstance()->set_wireframe_size(wireframe_size);
                }
                ImGui::PopID();

                ImGui::PushID("Vertex Only");
                ImGui::Text("Vertex size");
                float vertex_only_size = GlobalViewerSettings::getInstance()->get_vertex_only_size();
                if (ImGui::SliderFloat("##Size", &vertex_only_size, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic))
                {
                    GlobalViewerSettings::getInstance()->set_vertex_only_size(vertex_only_size);
                }
                ImGui::PopID();
            }

            bool transparency = GlobalViewerSettings::getInstance()->get_transparency_activated();
            ImGui::Checkbox("Transparency", &transparency);
            GlobalViewerSettings::getInstance()->set_transparency_activated(transparency);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates transparency");


            if (transparency)
            {
                if (ImGui::Button("Transparency Settings"))
                {
                    ImGui::OpenPopup("transparency Popup");
                }
                ImGui::SameLine();
                Tooltips::HelpMarkerWithQuestionMark(
                        "This button will open a Popup where you can switch the transparency mode between Weighted blended and Depth Peeling."
                        "It is also possible to adjust the number of passes used for depth peeling (default-value is 12)");
                if (ImGui::BeginPopup("transparency Popup"))
                {
                    int m_transparency = GlobalViewerSettings::getInstance()->get_transparency_mode();
                    if (ImGui::RadioButton("Weighted Blended", m_transparency == WEIGHTED_BLENDED))
                    {
                        GlobalViewerSettings::getInstance()->set_transparency_mode(WEIGHTED_BLENDED);
                    }
                    if (ImGui::RadioButton("Depth Peeling", m_transparency == DEPTH_PEELING))
                    {
                        GlobalViewerSettings::getInstance()->set_transparency_mode(DEPTH_PEELING);

                    }

                    if(m_transparency == DEPTH_PEELING)
                    {
                        int num_passes = GlobalViewerSettings::getInstance()->get_number_passes();
                        ImGui::SliderInt("DP_Passes", &num_passes, 0, 50);
                        GlobalViewerSettings::getInstance()->set_number_passes(num_passes);
                    }
                    ImGui::EndPopup();
                }
            }


            bool shadows = GlobalViewerSettings::getInstance()->get_shadows_activated();
            ImGui::Checkbox("Shadows", &shadows);
            GlobalViewerSettings::getInstance()->set_shadows_activated(shadows);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates shadows");
            if(shadows)
            {
                int cascades = GlobalViewerSettings::getInstance()->get_cascade_level();
                ImGui::SliderInt("Cascades", &cascades, 1, 8);
                GlobalViewerSettings::getInstance()->set_cascade_level(cascades);
            }

            if (mesh_mode == Wireframe || mesh_mode == Only_Vertices)
            {
                ImGui::End();
                return;
            }

            bool ambient_occlusion = GlobalViewerSettings::getInstance()->get_ambient_occlusion_activated();
            ImGui::Checkbox("Ambient Occlusion", &ambient_occlusion);
            GlobalViewerSettings::getInstance()->set_ambient_occlusion_activated(ambient_occlusion);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates ambient occlusion");


            if (ambient_occlusion)
            {
                ImVec2& padding = ImGui::GetStyle().FramePadding;
                //ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y * 2.0f});
                //ImGui::Text("Ambient Occlusion");
                //ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                int m_selected_preset = GlobalViewerSettings::getInstance()->get_ssao_options();
                ImGui::Combo(
                        "##Preset",
                        &m_selected_preset,
                        dropdown_presets,
                        IM_ARRAYSIZE(dropdown_presets),
                        IM_ARRAYSIZE(dropdown_presets));
                GlobalViewerSettings::getInstance()->set_ssao_options(m_selected_preset);
                // custom options when users want to tweak the values themselves
                if (m_selected_preset == CUSTOM)
                {
                    SSAOOptions actual_options = GlobalViewerSettings::getInstance()->get_custom_options();
                    actual_options.active = true;
                    ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                    ImGui::SliderInt("Samples", &actual_options.num_samples, 1, s_max_samples);
                    ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                    ImGui::SliderFloat("Radius", &actual_options.sample_radius, 0.0f, 3.0f);
                    ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                    ImGui::SliderFloat("Strength", &actual_options.strength, 0.0, 10.0);
                    ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                    ImGui::SliderFloat("Bias", &actual_options.z_bias, 0.0f, 0.1f);
                    GlobalViewerSettings::getInstance()->set_custom_options(actual_options);

                }
            }
        }
        ImGui::End();
    }

} // namespace volumeshOS
