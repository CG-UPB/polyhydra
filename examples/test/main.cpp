
#include <polyhydra/fs/FileManager.h>
#include <polyhydra/polyhydra.h>
#include <OpenVolumeMesh/Mesh/TetrahedralMesh.hh>
#include <random>

int main(int argc, char* argv[])
{
    std::vector<OpenVolumeMesh::CellHandle> selected;
    std::vector<polyhydra::VCylinder> cylinders;
    float r = 0.05f;

    auto add_cylinders = [&r](polyhydra::VMesh& mesh)
    {
        auto& ovm = *mesh.get_ovm();
        auto normals = OpenVolumeMesh::NormalAttrib<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>(ovm);
        normals.update_vertex_normals();

        for (auto c : ovm.cells())
        {
            for (auto e_it : ovm.cell_edges(c))
            {
                if (ovm.is_boundary(e_it))
                {
                    auto edge = ovm.edge(e_it);
                    auto from = ovm.vertex(edge.from_vertex());
                    auto to = ovm.vertex(edge.to_vertex());

                    auto dir = to - from;
                    auto pos = from + (dir) / 2.0f;

                    auto cylinder = mesh.add_shape<polyhydra::VCylinder>(c);
                    cylinder.set_position(pos);
                    cylinder.set_scale(r, glm::length(glm::vec3(dir[0], dir[1], dir[2])), r);
                    cylinder.set_direction(dir);
                }
            }
        }

        polyhydra::log("Added Cylinders");
    };

    polyhydra::set_theme(polyhydra::Theme::Dark);
    polyhydra::on_gui_render(
        [&cylinders, &r, &add_cylinders]()
        {
            ImGui::Begin("MyPanel");
            if (ImGui::Button("Load Mesh"))
            {
                auto mesh = polyhydra::load_from_dialog("Select OVM file");
            }
            auto mesh = polyhydra::get_focused_mesh();
            if (ImGui::Button("Add Cylinders") && mesh.is_valid())
            {
                polyhydra::remove_shapes();
                add_cylinders(mesh);
            }

            if (ImGui::Button("Remove shapes"))
            {
                polyhydra::remove_shapes();
                polyhydra::warn("Removed Shapes");
            }

            if (ImGui::Button("Randomize boundary color") && mesh.is_valid())
            {
                auto& ovm = *mesh.get_ovm();
                for (auto c : ovm.cells())
                {
                    for (auto f : ovm.cell_faces(c))
                    {
                        if (ovm.is_boundary(f))
                        {
                            double alpha = (float)rand() / (float)RAND_MAX;
                            auto color = glm::vec3(0.0f);
                            for (int i = 0; i < 3; i++)
                            {
                                color[i] = (float)rand() / (float)RAND_MAX;
                            }

                            for (auto hf : ovm.face_halffaces(f))
                            {
                                mesh.set_color(hf, glm::vec4{color[0], color[1], color[2], alpha});
                            }
                        }
                    }
                }
                mesh.use_base_color(false);
                polyhydra::use_transparency(true);
                polyhydra::set_transparency_passes(4);
            }

            if (ImGui::DragFloat("Cylinder Radius", &r, 0.1f, 0.1f, 0.5f, "%.1f") && mesh.is_valid())
            {
                polyhydra::remove_shapes();
                add_cylinders(mesh);
            }
            ImGui::End();
        });

    polyhydra::on_cell_select(
        [&](const polyhydra::VMesh vmesh, OpenVolumeMesh::CellHandle ch)
        {
            if (std::find(selected.begin(), selected.end(), ch) != selected.end())
            {
                vmesh.deselect(ch);
                selected.erase(std::remove(selected.begin(), selected.end(), ch), selected.end());
            }
            else
            {
                vmesh.select(ch);
                selected.push_back(ch);
            }

            polyhydra::log("Cell " + std::to_string(ch.uidx()) + " was selected");
        });
    polyhydra::on_halfface_select(
        [&](const polyhydra::VMesh vmesh, OpenVolumeMesh::HalfFaceHandle hf)
        {
            vmesh.select(hf);
            polyhydra::log("Halfface " + std::to_string(hf.uidx()) + " was selected");
        });
    polyhydra::on_position_select([&](float x, float y, float z) { polyhydra::log("Position selected"); });

    polyhydra::use_log_window(false);

    auto ovm_mesh = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>();
    OpenVolumeMesh::IO::FileManager file_manager;
    std::string path
        = polyhydra::Internal::FileManager::get_resource_path().string() + "/sample_meshes/bunny5824.1.ovm";
    file_manager.readFile(path, ovm_mesh);

    auto vmesh = polyhydra::load(&ovm_mesh, "Bunny from file");

    polyhydra::set_camera_mode(polyhydra::CameraMode::ORBIT);

    polyhydra::open();
}