
#include "volumeshOS.h"

void test_functionality(volumeshOS::VMesh mesh);

int main(int argc, char* argv[])
{
    using namespace volumeshOS;

    std::vector<VCylinder> cylinders;
    float r = 0.05f;

    set_theme(volumeshOS::Theme::Dark);
    on_gui_render([&cylinders, &r](){
        ImGui::Begin("MyPanel");
        if (ImGui::Button("Load Mesh"))
        {
            load_from_dialog("Select OVM file");
        }
        auto mesh = volumeshOS::get_focused_mesh();
        if (ImGui::Button("Add Cylinders") && mesh.is_valid())
        {
            auto& ovm = *mesh.get_ovm();
            auto normals = OpenVolumeMesh::NormalAttrib<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>(ovm);
            normals.update_vertex_normals();


            for (auto c_it : ovm.cells())
            {
                for(auto e_it : ovm.cell_edges(c_it))
                {
                    if(ovm.is_boundary(e_it))
                    {
                        auto edge = ovm.edge(e_it);
                        auto from = ovm.vertex(edge.from_vertex());
                        auto to = ovm.vertex(edge.to_vertex());

                        auto dir = to - from;
                        auto pos = from + (dir) / 2.0f;

                        auto cylinder = mesh.add_shape<VCylinder>(c_it);
                        cylinder.set_position(pos);
                        cylinder.set_scale(0.1f, glm::length(glm::vec3(dir[0], dir[1], dir[2])), 0.1f);
                        cylinder.set_direction(dir);
                    }
                }
            }

            log("Added Cylinders");
        }

        if (ImGui::Button("Remove shapes"))
        {
            remove_shapes();
            warn("Removed Shapes");
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
                        mesh.set_color(hf_it, glm::vec4{abs(normal[0]), abs(normal[1]), abs(normal[2]), 1.0f});
                    }
                }
            }
        }

        if(ImGui::DragFloat("Cylinder Radius", &r, 0.1f, 0.1f, 0.5f, "%.1f"))
        {
            for(auto cylinder : cylinders)
            {
                auto scale = cylinder.get_scale<glm::vec3>();
                cylinder.set_scale(r, scale.y, r);
            }
        }
        ImGui::End();
    });

    on_cell_select([&](const VMesh vmesh, OpenVolumeMesh::CellHandle ch){
        vmesh.select(ch);
        log("Cell " + std::to_string(ch.uidx()) + " was selected");
    });
    on_halfface_select([&](const VMesh vmesh, OpenVolumeMesh::HalfFaceHandle hf){
        vmesh.select(hf);
        log("Halfface " + std::to_string(hf.uidx()) + " was selected");
    });

    use_log_window(false);
    //set_selection_mode(SelectionMode::CELL);

    if (argc > 1)
    {
        auto mesh = load(argv[1]);
        //mesh.set_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    
    set_camera_mode(CameraMode::ORBIT);

    //test_functionality(mesh);

    open();
}

void test_functionality(volumeshOS::VMesh mesh)
{
    using namespace volumeshOS;

    use_shadows(false);
    use_ambient_occlusion(false);
    use_transparency(true);
    use_ambient_occlusion(false);

    mesh.set_position(0.0f, -1.43f, 0.0f);
    mesh.set_scale(1.2f);

    mesh.set_slice_factor(0.2f);
    mesh.set_slice_lock(true);

    mesh.set_cell_rounding(0.0f);
    mesh.set_cell_size(0.5f);

}
