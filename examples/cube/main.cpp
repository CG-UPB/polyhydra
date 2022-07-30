
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
        if (ImGui::Button("Add shapes") && mesh.is_valid())
        {
            auto& ovm = *mesh.get_ovm();
            for (auto c_it : ovm.cells())
            {
                for (auto hf_it : ovm.cell_halffaces(c_it))
                {
                    auto face = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(hf_it);
                    if (ovm.is_boundary(face))
                    {
                        auto shape = mesh.add_shape<volumeshOS::VSphere>(c_it);
                        shape.set_position(ovm.barycenter(face));
                        shape.set_scale(0.2f);
                    }
                }
            }
        }
        if (ImGui::Button("Remove shapes"))
        {
            volumeshOS::remove_shapes();
        }
        ImGui::End();
    });
    volumeshOS::on_cell_select([](auto vmesh, auto cell){
        vmesh.set_color(cell, glm::vec4{1.0f, 0.1f, 0.1f, 1.0f});
    });
    volumeshOS::open();
}
