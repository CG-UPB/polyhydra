#include "../Window.h"
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "../panels/NewFileDialog.h"

static bool light_mode = true;

void ui()
{
    vOS::Window &window = vOS::Window::instance();
    ImGui::Begin("Custom UI");
    // button to load a mesh into the window
    ImGui::Text("Load Mesh");
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        vOS::NewFileDialog file_dialog;
        char const *filename;
        filename = file_dialog.openDialog("Open Mesh File");
        if (filename != nullptr)
        {
            OpenVolumeMesh::GeometricPolyhedralMeshV3d mesh;
            OpenVolumeMesh::IO::FileManager file_manager;
            file_manager.readFile(filename, mesh);
            window.add_mesh(&mesh);
        }
    }

    // switch between light and dark mode
    ImGui::Separator();
    ImGui::Text("Light/DarkMode");
    ImGui::SameLine();
    if (ImGui::Button("Switch"))
    {
        if(light_mode)
        {
            window.load_dark_mode();
            light_mode = false;
        }
        else
        {
            window.load_light_mode();
            light_mode = true;
        }
    }
    ImGui::Separator();
    ImGui::Text("Light/DarkMode");
    ImGui::SameLine();

    ImGui::End();
}

void run()
{
    // VOS Window
    vOS::Window &window = vOS::Window::instance();
    window.set_custom_imgui([]{ return ui(); });
    window.open();
}

int main()
{
    run();
    return 0;
}