
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
        if (ImGui::Button("Add Arrows") && mesh.is_valid())
        {
            auto& ovm = *mesh.get_ovm();
            for (auto c_it : ovm.cells())
            {
                for (auto hf_it : ovm.cell_halffaces(c_it))
                {
                    auto face = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(hf_it);
                    if (ovm.is_boundary(face))
                    {
                        auto arrow = mesh.add_shape<volumeshOS::VArrow>(c_it);
                        auto center = ovm.barycenter(face);
                        auto normal = -ovm.normal(hf_it);
                        auto n = glm::vec3{normal[0], normal[1], normal[2]};
                        auto color = glm::mix(n, glm::vec3{0.5f}, 0.5f);
                        arrow.set_position(center);
                        arrow.set_scale(0.1f, 0.5f, 0.1f);
                        arrow.set_base_width(0.4f);
                        arrow.set_tip_height(0.15f);
                        arrow.set_direction(normal);
                        arrow.set_color(glm::vec4{color, 1.0f});
                    }
                }
            }
        }
        if (ImGui::Button("Remove shapes"))
        {
            volumeshOS::remove_shapes();
        }
        if (ImGui::Button("Set boundary color") && mesh.is_valid())
        {
            auto& ovm = *mesh.get_ovm();
            for (auto c_it : ovm.cells())
            {
                for (auto hf_it : ovm.cell_halffaces(c_it))
                {
                    auto face = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(hf_it);
                    if (ovm.is_boundary(face))
                    {
                        auto center = ovm.barycenter(face);
                        auto normal = -ovm.normal(hf_it);
                        mesh.set_color(hf_it, glm::vec4{normal[0], normal[1], normal[2], 0.5f});
                    }
                }
            }
        }
        ImGui::End();
    });
    volumeshOS::open();
}
