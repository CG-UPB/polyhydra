
#include "QualityPanel.h"
#include "../input/Input.h"
#include "../util/Tooltips.h"

namespace volumeshOS::Internal
{
    void QualityPanel::show() {
        if (ImGui::Begin("Graphics"))
        {
            auto& settings = AppState::settings;
            RenderingMode rendering_mode = settings.rendering_mode;
            if (rendering_mode == RenderingMode::WIREFRAME || rendering_mode == RenderingMode::ONLY_VERTICES){
                ImGui::TextColored(ImVec4(1.0f,0.0f,0.0f,1.0f),"Some graphics settings are not available, \n"
                                                               "since Wireframe or Vertex-Only mode is activated.");

                ImGui::PushID("Wireframe");
                ImGui::Text("Wireframe size");
                ImGui::SliderFloat("##Size", &settings.wireframe_size, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
                ImGui::PopID();

                ImGui::PushID("Vertex Only");
                ImGui::Text("Vertex size");
                ImGui::SliderFloat("##Size", &settings.vertex_size, 0.0f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
                ImGui::PopID();
            }
            ImGui::Checkbox("Transparency", &settings.transparency_active);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates transparency");

            if (settings.transparency_active)
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
                    auto transparency_mode = settings.transparency_mode;

                    if (ImGui::RadioButton("Depth Peeling", transparency_mode == TransparencyMode::DEPTH_PEELING))
                    {
                        settings.transparency_mode = TransparencyMode::DEPTH_PEELING;
                    }
                    if (transparency_mode == TransparencyMode::DEPTH_PEELING)
                    {
                        ImGui::SliderInt("DP_Passes", &settings.num_depth_peeling_passes, 0, 50);
                    }
                    if (ImGui::RadioButton("Weighted Blended", transparency_mode == TransparencyMode::WEIGHTED_BLENDED))
                    {
                        settings.transparency_mode = TransparencyMode::WEIGHTED_BLENDED;
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::Checkbox("Shadows", &settings.shadows_active);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates shadows");
            if (settings.shadows_active)
            {
                ImGui::SliderInt("Cascades", &settings.num_shadow_cascades, 1, 8);
            }

            if (rendering_mode == RenderingMode::WIREFRAME || rendering_mode == RenderingMode::ONLY_VERTICES)
            {
                ImGui::End();
                return;
            }

            ImGui::Checkbox("Ambient Occlusion", &settings.ssao_active);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates ambient occlusion");

            if (settings.ssao_active)
            {
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
        }
        ImGui::End();
    }

} // namespace volumeshOS
