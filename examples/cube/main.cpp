
#include "volumeshOS.h"
#include "panels/NewFileDialog.h"

int main()
{
    //volumeshOS::VMesh mesh;
//    volumeshOS::on_gui_render([&mesh](){
//        ImGui::Begin("MyPanel");
//        if (ImGui::Button("Load Mesh"))
//        {
//            if (auto file = volumeshOS::file_dialog("Select ovm file"))
//            {
//                mesh.update(file);
//                //mesh = volumeshOS::update(file);
//                volumeshOS::update(mesh, file);
//            }
//        }
//        ImGui::End();
//    });
//    volumeshOS::on_cell_select([](auto mesh, auto cell){
//        mesh.isolate(cell);
//        mesh.load_configuration("");
//        mesh.set_color(cell, glm::vec3());
//    });


    //auto file = "/home/lukas/CLionProjects/volumeshos/res/OVM/Hex/nut_el0_5_hex_opt.ovm";
    //volumeshOS::update(mesh, file);

    volumeshOS::initialize();

    auto fd = volumeshOS::Internal::NewFileDialog();
    auto file = fd.open_dialog("ok");
    if (file != nullptr)
    {
        std::string f = std::string(file);
        auto mesh = volumeshOS::load(f);
    }


    volumeshOS::open();
}
