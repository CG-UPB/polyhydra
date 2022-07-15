
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
        }
        ImGui::End();
    });
    volumeshOS::open();
}
