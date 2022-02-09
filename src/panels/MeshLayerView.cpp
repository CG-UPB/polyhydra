//
// Created by projektgruppe on 14.01.22.
//

#include "MeshLayerView.h"
#include "../Window.h"
#include "imgui.h"
#include <imgui_internal.h>

namespace vOS
{

    MeshLayerView* MeshLayerView::instance = 0;

    // Singleton
    MeshLayerView* MeshLayerView::getInstance()
    {
        if (instance == 0)
        {
            instance = new MeshLayerView();
        }

        return instance;

    }

    MeshLayerView::MeshLayerView() {
    }

    // Destruktor
    MeshLayerView::~MeshLayerView()
    {
        //delete instance;
    }



    // Helper to display a little (?) mark which shows a tooltip when hovered.
    // In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
    static void HelpMarkerWithQuestionMark(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    // show log window and corresponding buttons
    void MeshLayerView::show()
    {
        if(!ImGui::Begin("Meshes"))
        {
            ImGui::End();
            return;
        }
        int active_mesh = Window::instance().get_mesh_active();
        for(const std::pair<int, MeshObject*> m : Window::instance().get_mesh_list())
        {
            std::string str = "Mesh " + std::to_string(m.first);
            char* char_type = new char[str.length()];

            ImGui::RadioButton(strcpy(char_type, str.c_str()),&active_mesh, m.first);
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
            {
                LogWindow::getInstance()->addLog("New Focus Mesh");
                Window::instance().rendering_mutex.unlock();
                Window::instance().set_focus_mesh(m.first);
                Window::instance().rendering_mutex.lock();
            }


            if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("These Radio Buttons show which Mesh is active now. The filter functions of "
                                       "the Toolbar will change only the active mesh");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }

            //ImGui::TextColored(ImColor(ImVec4(1.0,0.0,0.0,1.0)),strcpy(char_type, str.c_str()));ImGui::SameLine();

            bool visible = Window::instance().get_mesh_visibility(m.first);
            str = "##Visible " + std::to_string(m.first);
            char_type = new char[str.length()];
            ImGui::SameLine();ImGui::Checkbox(strcpy(char_type, str.c_str()), &visible);ImGui::SameLine();
            Window::instance().rendering_mutex.unlock();
            Window::instance().set_mesh_visibility(m.first, visible);
            Window::instance().rendering_mutex.lock();
            if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("If the Checkbox is clicked, the mesh is visible");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }

            Color color = Window::instance().get_mesh_color(m.first);
            float m_color[4];
            m_color[0] = color.get_rgba().r;
            m_color[1] = color.get_rgba().g;
            m_color[2] = color.get_rgba().b;
            m_color[3] = color.get_rgba().a;
            str = "Color " + std::to_string(m.first);
            char_type = new char[str.length()];
            ImGui::ColorEdit4(strcpy(char_type, str.c_str()),m_color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
            Window::instance().rendering_mutex.unlock();
            Window::instance().set_mesh_color(m.first, Color(m_color[0], m_color[1], m_color[2], m_color[3]));
            Window::instance().rendering_mutex.lock();
            if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("Sets the color of the mesh");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }

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
            if(ImGui::IsItemHovered()  && GImGui->HoveredIdTimer > m_timer_treshold)
            {
                ImGui::BeginTooltip();
                ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                ImGui::TextUnformatted("Sets the rendering mode of the mesh");
                ImGui::PopTextWrapPos();
                ImGui::EndTooltip();
            }
        }
        Window::instance().rendering_mutex.unlock();
        Window::instance().set_mesh_active(active_mesh);
        Window::instance().rendering_mutex.lock();

        ImGui::End();
    }
} // namespace vOS