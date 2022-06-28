
#include "MeshLayerView.h"
#include "../Window.h"
#include "../util/Tooltips.h"
#include "NewFileDialog.h"
#include "../util/ImGuiUtil.h"

namespace volumeshOS::Internal
{


    MeshLayerView::MeshLayerView() {
        // empty constructor
    }

    // Destruktor
    MeshLayerView::~MeshLayerView()
    {
        //delete instance;
    }



    void MeshLayerView::show()
    {
        if(!ImGui::Begin("Meshes"))
        {
            ImGui::End();
            return;
        }

        ImGui::PushStyleColor(ImGuiCol_Separator, ImGui::GetStyleColorVec4(ImGuiCol_Button));

        // create a line for every loaded mesh
        int active_mesh = Window::instance().get_mesh_focus();
        for(const auto& [id, mesh] : Window::instance().get_mesh_list())
        {
            ImGui::PushID(id);
            // name is "Mesh" with unique ID
            std::string str = "Mesh " + std::to_string(id);
            ImGui::RadioButton(str.c_str(),&active_mesh, id);
            // if radiobutton is double-clicked, set actual mesh in focus
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                LogWindow::getInstance()->addLog("New Focus Mesh");
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_focus(id);
                Window::instance().rendering_mutex.lock();
            }

            Tooltips::ToolTipByHovering("These Radio Buttons show which Mesh is active now. The filter functions of "
                                        "the Toolbar will change only the active mesh");

            ImGui::SameLine(ImGui::GetWindowWidth() - 135.0f);

            bool visible = Window::instance().get_mesh_visibility(id);
            ImGui::Checkbox("##Visible", &visible);
            ImGui::SameLine();
            Window::instance().set_mesh_visibility(id, visible);
            Tooltips::ToolTipByHovering("If the Checkbox is clicked, the mesh is visible");


            Color color = Window::instance().get_mesh_color(id);
            float m_color[4];
            m_color[0] = color.get_rgba().r;
            m_color[1] = color.get_rgba().g;
            m_color[2] = color.get_rgba().b;
            m_color[3] = color.get_rgba().a;
            ImGui::ColorEdit4("Color",m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
            Window::instance().set_mesh_color(id, Color(m_color[0], m_color[1], m_color[2], m_color[3]));
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

                    filename = file_dialog.saveMeshSettings("Save Mesh Settings File");

                    if (filename != nullptr)
                    {
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().save_mesh_data(id, filename);
                        Window::instance().rendering_mutex.lock();
                    }
                }
                ImGui::SameLine();
                // Load Mesh Settings to File
                if (ImGui::Button("Load")) {
                    NewFileDialog file_dialog;

                    char const *filename;

                    filename = file_dialog.loadMeshSettings("Save Mesh Settings File");
                    if (filename != nullptr)
                    {
                        Window::instance().rendering_mutex.unlock();
                        Window::instance().load_mesh_data(id, filename);
                        Window::instance().rendering_mutex.lock();
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

                int active_mesh = id;
                // Ambient
                ImGui::Text("Ambient:");
                float ambient_value = Window::instance().get_mesh_ambient_strength(active_mesh);
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Ambient", &ambient_value, 0.0f, 1.0f);
                Window::instance().set_mesh_ambient_strength(active_mesh, ambient_value);

                // Diffuse
                ImGui::Text("Diffuse:");
                float diffuse_value = Window::instance().get_mesh_diffuse_strength(active_mesh);
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Diffuse", &diffuse_value, 0.0f, 1.0f);
                Window::instance().set_mesh_diffuse_strength(active_mesh, diffuse_value);

                // Specular
                ImGui::Text("Specular:");
                float specular_value = Window::instance().get_mesh_specular_strength(active_mesh);
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Specular", &specular_value, 0.0f, 1.0f);
                Window::instance().set_mesh_specular_strength(active_mesh, specular_value);

                // Specular Exponent
                ImGui::Text("Specular Exponent:");
                float specular_exp = Window::instance().get_mesh_specular_exponent(active_mesh);
                ImGui::SetNextItemWidth(slider_width);
                ImGui::SameLine(ImGui::GetWindowWidth() - slider_width - padding_right);
                ImGui::SliderFloat("##Specular Exponent", &specular_exp, 0.0f, 10.0f);
                Window::instance().set_mesh_specular_exponent(active_mesh, specular_exp);
                ImGui::EndPopup();
            }
            ImGui::Separator();
            ImGui::PopID();
        }
        Window::instance().rendering_mutex.unlock();
        Window::instance().set_mesh_focus(active_mesh);
        Window::instance().rendering_mutex.lock();

        ImGui::PopStyleColor();

        ImGui::End();
    }
} // namespace volumeshOS