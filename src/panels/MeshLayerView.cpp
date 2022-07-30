
#include "MeshLayerView.h"
#include "../util/Tooltips.h"
#include "NewFileDialog.h"
#include "../util/ImGuiUtil.h"
#include "volumeshOS.h"

namespace volumeshOS::Internal
{
    void MeshLayerView::show()
    {
        if(!ImGui::Begin("Meshes"))
        {
            ImGui::End();
            return;
        }

        ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));

        // create a line for every loaded mesh
        auto active_mesh = volumeshOS::get_focused_mesh();
        int active_mesh_id = active_mesh.get_id();
        for(const auto& mesh : volumeshOS::get_meshes())
        {
            int id = mesh.get_id();
            ImGui::PushID(id);
            if(ImGui::RadioButton(mesh.get_name().c_str(), &active_mesh_id, id))
            {
                volumeshOS::set_focused_mesh(VMesh(active_mesh_id));
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                volumeshOS::focus_camera(VMesh(active_mesh_id));
            }

            Tooltips::ToolTipByHovering("These Radio Buttons show which Mesh is active now. The filter functions of "
                                        "the Toolbar will change only the active mesh");

            ImGui::SameLine(ImGui::GetWindowWidth() - 135.0f);

            bool visible = mesh.get_visibility();
            if(ImGui::Checkbox("##Visible", &visible))
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
            ImGui::ColorEdit4("Color", new_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
            mesh.set_color(glm::vec4{new_color[0], new_color[1], new_color[2], new_color[3]});
            Tooltips::ToolTipByHovering("Sets the color of the mesh");

            // Advanced Settings
            if (ImGuiUtil::icon_button("settings.png"))
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
                if (ImGui::Button("Save")) {
                    NewFileDialog file_dialog;

                    char const *filename;

                    filename = file_dialog.save_mesh_settings("Save Mesh Settings File");

                    if (filename != nullptr)
                    {
                        mesh.save_configuration(filename);
                    }
                }
                ImGui::SameLine();
                // Load Mesh Settings to File
                if (ImGui::Button("Load")) {
                    NewFileDialog file_dialog;

                    char const *filename;

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
                ImGui::Text("Material");
                ImGui::PopFont();
                ImGuiUtil::add_padding_y(0.5f);

                // Phong Settings

                float slider_width = 200.0f;
                float padding_right = 20.0f;

                // Ambient
                ImGui::Text("Ambient:");
                float ambient_value = mesh.get_ambient();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Ambient", &ambient_value, 0.0f, 1.0f);
                mesh.set_ambient(ambient_value);

                // Diffuse
                ImGui::Text("Diffuse:");
                float diffuse_value = mesh.get_diffuse();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Diffuse", &diffuse_value, 0.0f, 1.0f);
                mesh.set_diffuse(diffuse_value);

                // Specular
                ImGui::Text("Specular:");
                float specular_value = mesh.get_specular();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Specular", &specular_value, 0.0f, 1.0f);
                mesh.set_specular(specular_value);

                // Specular Exponent
                ImGui::Text("Specular Exponent:");
                float specular_exp = mesh.get_specular_coefficient();
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Specular Exponent", &specular_exp, 0.0f, 10.0f);
                mesh.set_specular_coefficient(specular_exp);
                ImGui::EndPopup();
            }
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::PopStyleColor();
        ImGui::End();
    }
} // namespace volumeshOS