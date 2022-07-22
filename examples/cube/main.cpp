
#include "volumeshOS.h"

int main()
{
    volumeshOS::VMesh mesh;
    volumeshOS::on_gui_render([&mesh](){
        ImGui::Begin("MyPanel");
        if (ImGui::Button("Load Mesh"))
        {
            mesh = volumeshOS::load_from_dialog("Select OVM file");
        }
        if (ImGui::Button("Run Algorithm") && mesh.is_valid())
        {
            volumeshOS::log("Running algorithm");
            volumeshOS::focus_camera(mesh);
        }
        ImGui::End();
    });
    volumeshOS::on_cell_select([](auto vmesh, auto cell){
        vmesh.set_color(cell, {1.0f, 0.1f, 0.1f});
    });
    volumeshOS::open();
}
