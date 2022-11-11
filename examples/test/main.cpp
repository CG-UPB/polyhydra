
#include "volumeshOS.h"
#include "OpenVolumeMesh/Mesh/TetrahedralMesh.hh"
#include <random>

void test_functionality(volumeshOS::VMesh mesh);

int main(int argc, char* argv[])
{
    using namespace volumeshOS;

    std::vector<OpenVolumeMesh::CellHandle> selected;
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
                for (auto f_it : ovm.cell_faces(c_it))
                {
//                    auto face = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(hf_it);
//                    if (ovm.is_boundary(face))
//                    {
//                        auto center = ovm.barycenter(face);
//                        auto normal = -ovm.normal(hf_it);
//                        mesh.set_color(hf_it, glm::vec4{abs(normal[0]), abs(normal[1]), abs(normal[2]), 1.0f});
//                    }
                    double alpha = (float)rand() / (float)RAND_MAX;
                    std::cout << alpha << std::endl;
                    alpha = alpha > 0.2 ? 1.0 : 0.0;
                    auto color = glm::vec3(0.0f);
                    for(int i = 0; i < 3; i++)
                    {
                        color[i] = (float)rand() / (float)RAND_MAX;
                    }

                    for(auto hf_it : ovm.face_halffaces(f_it))
                    {
                        mesh.set_color(hf_it, glm::vec4{color[0], color[1], color[2], alpha});
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
        if(std::find(selected.begin(), selected.end(), ch) != selected.end())
        {
            vmesh.deselect(ch);
            selected.erase(std::remove(selected.begin(), selected.end(), ch), selected.end());
        }
        else
        {
            vmesh.select(ch);
            selected.push_back(ch);
        }


        log("Cell " + std::to_string(ch.uidx()) + " was selected");
    });
    on_halfface_select([&](const VMesh vmesh, OpenVolumeMesh::HalfFaceHandle hf){
        vmesh.select(hf);
        log("Halfface " + std::to_string(hf.uidx()) + " was selected");
    });
    on_position_select([&](float x, float y, float z){
        log("Position selected");
    });

    use_log_window(false);
    //set_selection_mode(SelectionMode::CELL);

    if (argc > 1)
    {
        auto mesh = load(argv[1]);
        //mesh.set_color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }
    OpenVolumeMesh::GeometricTetrahedralMeshV3d myMesh;
    std::vector<OpenVolumeMesh::VertexHandle> vertices;
    vertices.push_back(myMesh.add_vertex(OpenVolumeMesh::Vec3d(1,0,0)));
    vertices.push_back(myMesh.add_vertex(OpenVolumeMesh::Vec3d(0,1,0)));
    vertices.push_back(myMesh.add_vertex(OpenVolumeMesh::Vec3d(0,0,1)));
    vertices.push_back(myMesh.add_vertex(OpenVolumeMesh::Vec3d(1,0,1)));
    myMesh.add_cell(vertices);
    load(&myMesh);

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
    mesh.set_slice_locked(true);

    mesh.set_cell_rounding(0.0f);
    mesh.set_cell_size(0.5f);

}
