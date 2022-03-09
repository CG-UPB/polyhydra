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
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y * 2.0f});
            ImGui::Text("Ambient Occlusion");
            ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
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
                SSAOOptions actual_options = GlobalViewerSettings::getInstance()->m_get_current_costume_options();
                actual_options.active = true;
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderInt("Samples", &actual_options.num_samples, 1, s_max_samples);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderFloat("Radius", &actual_options.sample_radius, 0.0f, 3.0f);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderFloat("Strength", &actual_options.strength, 0.0, 10.0);
                ImGui::SetCursorPos({ImGui::GetCursorPosX() + padding.x, ImGui::GetCursorPosY() + padding.y});
                ImGui::SliderFloat("Bias", &actual_options.z_bias, 0.0f, 0.1f);
                GlobalViewerSettings::getInstance()->m_set_current_costume_options(actual_options);
            }
        }
        ImGui::End();
    }

} // namespace vOS
