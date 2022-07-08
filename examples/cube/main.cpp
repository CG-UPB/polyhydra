
#include "volumeshOS.h"

int main()
{
    volumeshOS::VMesh mesh;
    volumeshOS::on_gui_render([&mesh](){
        ImGui::Begin("MyPanel");
        if (ImGui::Button("Load Mesh"))
        {
            if (auto file = volumeshOS::file_dialog("Select ovm file"))
            {
                mesh.update(file);
                //mesh = volumeshOS::update(file);
                volumeshOS::update(mesh, file);
            }
        }
        ImGui::End();
    });
    volumeshOS::on_cell_select([](auto mesh, auto cell){
        mesh.isolate(cell);
        mesh.load_configuration("");
        mesh.set_color(cell, glm::vec3());
    });
    volumeshOS::open();
}
