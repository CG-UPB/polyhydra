#include "QualityPanel.h"
#include "../input/Input.h"
#include "imgui.h"
#include "MeshView.h"
#include "NewFileDialog.h"
#include "../util/Tooltips.h"


namespace vOS
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
            ImVec2& padding = ImGui::GetStyle().FramePadding;
            //ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y * 2.0f});
            ImGui::Text("Ambient Occlusion");
            //ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
            int m_selected_preset = GlobalViewerSettings::getInstance()->m_get_current_ssao_option();
            ImGui::Combo(
                    "##Preset",
                    &m_selected_preset,
                    dropdown_presets,
                    IM_ARRAYSIZE(dropdown_presets),
                    IM_ARRAYSIZE(dropdown_presets));
            GlobalViewerSettings::getInstance()->m_set_current_ssao_option(m_selected_preset);
            // custom options when users want to tweak the values themselves
            if (m_selected_preset == CUSTOM)
            {
                SSAOOptions *actual_options = GlobalViewerSettings::getInstance()->m_get_current_costume_options();
                actual_options->active = true;
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderInt("Samples", &actual_options->num_samples, 1, s_max_samples);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderFloat("Radius", &actual_options->sample_radius, 0.0f, 3.0f);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderFloat("Strength", &actual_options->strength, 0.0, 10.0);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderFloat("Bias", &actual_options->z_bias, 0.0f, 0.1f);
                //GlobalViewerSettings::getInstance()->m_set_current_ao_options(*actual_options);
            }

            // Transparency Settings
            // TODO: Specify in which modes the transparency Settings are useful and add an if-statement
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
                int m_transparency = GlobalViewerSettings::getInstance()->m_get_current_transparency_mode();
                if (ImGui::RadioButton("Weighted Blended", m_transparency == WEIGHTED_BLENDED))
                {
                    GlobalViewerSettings::getInstance()->m_set_current_transparency_mode(WEIGHTED_BLENDED);
                }
                if (ImGui::RadioButton("Depth Peeling", m_transparency == DEPTH_PEELING))
                {
                    GlobalViewerSettings::getInstance()->m_set_current_transparency_mode(DEPTH_PEELING);

                }

                if(m_transparency == DEPTH_PEELING)
                {
                    int num_passes = GlobalViewerSettings::getInstance()->m_get_current_number_passes();
                    ImGui::SliderInt("DP_Passes", &num_passes, 0, 50);
                    GlobalViewerSettings::getInstance()->m_set_current_number_passes(num_passes);
                }
                ImGui::EndPopup();
            }

            bool rounding_active = GlobalViewerSettings::getInstance()->m_get_current_rounding_active();

            ImGui::Checkbox("Rounded Cells?", &rounding_active);
            GlobalViewerSettings::getInstance()->m_set_current_rounding_active(rounding_active);
            ImGui::SameLine();
            Tooltips::HelpMarkerWithQuestionMark("This checkbox activates rounded corners for the edges of the meshes");
            if (rounding_active)
            {
                float actual_rounding_size = GlobalViewerSettings::getInstance()->m_get_current_rounding_size();
                ImGui::SliderFloat("Size", &actual_rounding_size, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
                GlobalViewerSettings::getInstance()->m_set_current_rounding_size(actual_rounding_size);
            }

        }
        ImGui::End();
    }

} // namespace vOS
