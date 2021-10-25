
#include "MenuBar.h"

#include <imgui.h>

#include "ImGuiFileDialog.h"
#include <iostream>

#include "../mesh/mesh_object.h"
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

            ImGui::EndMenuBar();
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseOVMFIle", ImGuiWindowFlags_NoCollapse, ImVec2(400,200), ImVec2(1200,600))) {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();

                std::cout << filePathName << std::endl;
                // action
                //m_mesh->read_from_file(std::string filePathName);
                MeshObject *mesh = MeshObject::getInstance();
                mesh->load_from_file(filePathName);

                std::vector<unsigned int> edges = mesh->edges();
                int count = 0;
                for(auto it = std::begin(edges); it != std::end(edges); ++it) {
                    std::cout << *it << "\n";
                    count++;
                    LogWindow::getInstance()->addLog( std::to_string(*it).c_str() ,0);
                    if (count >= 100){
                        break;
                    }
                }


            }
            ImGuiFileDialog::Instance()->Close();
        }
    }
}
