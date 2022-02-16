//
// Created by projektgruppe on 14.01.22.
//

#include "MeshLayerView.h"
#include "../Window.h"
#include "imgui.h"
#include <imgui_internal.h>
#include "../util/Tooltips.h"

namespace vOS
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
        // create a line for every loaded mesh
        int active_mesh = Window::instance().get_mesh_focus();
        for(const std::pair<int, MeshObject*> m : Window::instance().get_mesh_list())
        {
            // name is "Mesh" with unique ID
            std::string str = "Mesh " + std::to_string(m.first);
            char* char_type = new char[str.length()];

            ImGui::RadioButton(strcpy(char_type, str.c_str()),&active_mesh, m.first);
            // if radiobutton is double-clicked, set actual mesh in focus
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                LogWindow::getInstance()->addLog("New Focus Mesh");
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_mesh_focus(m.first);
                Window::instance().rendering_mutex.lock();
            }

            Tooltips::ToolTipByHovering("These Radio Buttons show which Mesh is active now. The filter functions of "
                                        "the Toolbar will change only the active mesh");



            bool visible = Window::instance().get_mesh_visibility(m.first);
            str = "##Visible " + std::to_string(m.first);
            char_type = new char[str.length()];
            ImGui::SameLine();ImGui::Checkbox(strcpy(char_type, str.c_str()), &visible);ImGui::SameLine();
            Window::instance().rendering_mutex.unlock();
            Window::instance().set_mesh_visibility(m.first, visible);
            Window::instance().rendering_mutex.lock();
            Tooltips::ToolTipByHovering("If the Checkbox is clicked, the mesh is visible");

            Color color = Window::instance().get_mesh_color(m.first);
            float m_color[3];
            m_color[0] = color.get().r;
            m_color[1] = color.get().g;
            m_color[2] = color.get().b;
            str = "Color " + std::to_string(m.first);
            char_type = new char[str.length()];
            ImGui::ColorEdit4(strcpy(char_type, str.c_str()),m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
            Window::instance().rendering_mutex.unlock();
            Window::instance().set_mesh_color(m.first, Color(m_color[0], m_color[1], m_color[2], m_color[3]));
            Window::instance().rendering_mutex.lock();
            Tooltips::ToolTipByHovering("Sets the color of the mesh");


            // TODO: Update to an enum, so that its not hard coded here
            std::string current_rendering_mode = Window::instance().get_mesh_rendering_mode(m.first);
            int current_rendering_mode_int = 0;
            if (current_rendering_mode == "mesh_wireframe")
                current_rendering_mode_int = 1;
            if (current_rendering_mode == "mesh_normal")
                current_rendering_mode_int = 2;
            const char *rendering_mode_internal_names[] =
                    {
                            "mesh_phong", "mesh_wireframe", "mesh_normal"
                    };
            const char *rendering_mode_external_names[] =
                    {
                                "Phong", "Wireframe", "Normal"
                    };
            str = "##R-Mode " + std::to_string(m.first);
            char_type = new char[str.length()];
            ImGui::Combo(strcpy(char_type, str.c_str()), &current_rendering_mode_int, rendering_mode_external_names,
                         IM_ARRAYSIZE(rendering_mode_internal_names), IM_ARRAYSIZE(rendering_mode_internal_names));
            Window::instance().rendering_mutex.unlock();
            Window::instance().set_mesh_rendering_mode(m.first,rendering_mode_internal_names[current_rendering_mode_int]);
            Window::instance().rendering_mutex.lock();

            Tooltips::ToolTipByHovering("Sets the rendering mode of the mesh");

        }
        Window::instance().rendering_mutex.unlock();
        Window::instance().set_mesh_focus(active_mesh);
        Window::instance().rendering_mutex.lock();

        ImGui::End();
    }
} // namespace vOS