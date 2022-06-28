
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
                mesh = volumeshOS::load(file);
            }
        }
        ImGui::End();
    });
    volumeshOS::on_cell_select([](auto mesh, auto cell){
        mesh.isolate(cell);
        mesh.load_configuration("");
    });
    volumeshOS::open();
}
