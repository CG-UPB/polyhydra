
#include "MenuBar.h"

#include <imgui.h>

#include "ImGuiFileDialog.h"
#include <iostream>

#include "../algorithms/VosWindow.h"
#include "LogWindow.h"

namespace vOS
{
    void MenuBar::show()
    {
        // this should of course be changed to serve a function, for now this is just a placeholder
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {}

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {
                    ImGuiFileDialog::Instance()->OpenDialog("ChooseOVMFIle", "Choose File", ".ovm", ".");

                }


                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {}

                if (ImGui::MenuItem("Exit"))
                {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N"))
                {}

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                {}

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                {}

                if (ImGui::MenuItem("Exit"))
                {}
                ImGui::EndMenu();
            }
            ///                              Timeline Buttons

            // Pause Button
            if(m_pause_toggled)
            {
                // Pause button is active, pressing it would undo pause

                if(ImGui::Button(">"))
                {
                    m_pause_toggled = false;
                    VosWindow::instance().m_on_vos_unpaused();
                }
            }else{
                // Pause button is inactive, pressing it would pause

                if(ImGui::Button("||"))
                {
                    m_pause_toggled = true;
                        VosWindow::instance().m_on_vos_paused();
                }
            }
            // Reset Button
            if(ImGui::Button("Reset"))
            {
                    VosWindow::instance().m_on_reset();
            }

            // Step Button
            if(ImGui::Button("Step"))
            {
                VosWindow::instance().m_on_step();
            }

            ImGui::EndMenuBar();
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseOVMFIle", ImGuiWindowFlags_NoCollapse, ImVec2(400,200), ImVec2(1200,600)))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                std::cout << filePathName << std::endl;
                // action
                //VosWindow::instance().get_mesh_obj().load_from_file(filePathName);
                VosWindow::instance().set_loaded_file_path_name(filePathName);
            }
            ImGuiFileDialog::Instance()->Close();
        }

    }

    bool MenuBar::pause_is_pressed(){
        return m_pause_toggled;
    }
}
