#include "volumeshOS.h"

using namespace volumeshOS;

OpenVolumeMesh::GeometricPolyhedralMeshV3d gen_mesh()
{
    OpenVolumeMesh::GeometricPolyhedralMeshV3d mesh;

    // Create mesh object
    // Add eight vertices
    OpenVolumeMesh::VertexHandle v0 = mesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, -1.0, -1.0));
    OpenVolumeMesh::VertexHandle v1 = mesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, -1.0, -1.0));
    OpenVolumeMesh::VertexHandle v2 = mesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, 1.0, -1.0));
    OpenVolumeMesh::VertexHandle v3 = mesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, 1.0, -1.0));
    OpenVolumeMesh::VertexHandle v4 = mesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, -1.0, 1.0));
    OpenVolumeMesh::VertexHandle v5 = mesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, -1.0, 1.0));
    OpenVolumeMesh::VertexHandle v6 = mesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, 1.0, 1.0));
    OpenVolumeMesh::VertexHandle v7 = mesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, 1.0, 1.0));
    std::vector<OpenVolumeMesh::VertexHandle> vertices;
    // Add faces
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v3);
    vertices.push_back(v2);
    OpenVolumeMesh::FaceHandle f0 = mesh.add_face(vertices);
    vertices.clear();
    vertices.push_back(v1);
    vertices.push_back(v5);
    vertices.push_back(v7);
    vertices.push_back(v3);
    OpenVolumeMesh::FaceHandle f1 = mesh.add_face(vertices);
    vertices.clear();
    vertices.push_back(v5);
    vertices.push_back(v4);
    vertices.push_back(v6);
    vertices.push_back(v7);
    OpenVolumeMesh::FaceHandle f2 = mesh.add_face(vertices);
    vertices.clear();
    vertices.push_back(v4);
    vertices.push_back(v0);
    vertices.push_back(v2);
    vertices.push_back(v6);
    OpenVolumeMesh::FaceHandle f3 = mesh.add_face(vertices);
    vertices.clear();
    vertices.push_back(v0);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v1);
    OpenVolumeMesh::FaceHandle f4 = mesh.add_face(vertices);
    vertices.clear();
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v7);
    vertices.push_back(v6);
    OpenVolumeMesh::FaceHandle f5 = mesh.add_face(vertices);
    std::vector<OpenVolumeMesh::HalfFaceHandle> halffaces;
    // Add first tetrahedron
    halffaces.push_back(mesh.halfface_handle(f0, 0));
    halffaces.push_back(mesh.halfface_handle(f1, 0));
    halffaces.push_back(mesh.halfface_handle(f2, 0));
    halffaces.push_back(mesh.halfface_handle(f3, 0));
    halffaces.push_back(mesh.halfface_handle(f4, 0));
    halffaces.push_back(mesh.halfface_handle(f5, 0));
    mesh.add_cell(halffaces);

    return mesh;
}

void setup_mesh(volumeshOS::VMesh mesh, std::string name = "")
{
    // Note: None of these settings are necessary to set before open the viewer
    //       Most of them set the default value just for demonstration
    //       See volumeshOS.h for more information

    mesh.set_name(name);

    // these functions can be used to get insight of cells
    mesh.use_backface_culling(true);
    mesh.use_two_sided_lighting(false);

    // if true there is one color that can be set inside the viewer.
    // otherwise each cell/face has an own color value
    mesh.use_base_color(true);


    // example usage of mesh related functions
    mesh.set_position(0.0, 0.0, 0.0);
    mesh.set_scale(1.0);
    //mesh.set_rotation(...);

    mesh.set_color(std::array<float, 4>{0.0, 1.0, 0.0, 1.0});

    // settings for phong lighting
    mesh.set_ambient(1.0);
    mesh.set_diffuse(1.0);
    mesh.set_specular(0.15);
    mesh.set_specular_coefficient(8.0);

    // settings for physically based lighting (PBR)
    mesh.set_metallic(0.15);
    mesh.set_roughness(0.65);

    // set lighting mode: Phong or PBR
    mesh.set_lighting_mode(LightingMode::PBR);

    // slice into the mesh; direction is given by view direction
    mesh.set_slice_factor(0.0);
    mesh.set_slice_locked(false);
    // peel the mesh; opens inner layers;
    mesh.set_peel_level(0.0);
    // when true peeling is done from the inside to the outside
    mesh.use_reverse_peeling(false);
    // set cell size; moves cell vertices to its center
    mesh.set_cell_size(1.0);
    // rounds the edges of a cell
    mesh.set_cell_rounding(0.0);

    // set tessellation level (1-64) for bezier meshes:
    mesh.set_tessellation_level(1);
    mesh.set_visibility(true);


}

void setup_graphics()
{
    // Do further settings in the viewer

    // Shadows
    use_shadows(false);

    // Ambient Occlusion
    use_ambient_occlusion(false);

    //Transparency
    use_transparency(false);

    // Post Processing
    set_gamma(2.4);
    set_saturation(1.0);
    set_contrast(1.0);

}

int main(int argc, char* argv[])
{

    // generate ovm mesh
    auto ovm_mesh = gen_mesh();

    // load mesh into view; get VMesh object in return to operate on
    auto mesh = load(&ovm_mesh);
    setup_mesh(mesh, "Cube");

    setup_graphics();

    // Ground

    set_camera_mode(CameraMode::ORBIT);

    // define lambda function that gets executed each frame
    // use ImGUI to setup your own GUI
    on_gui_render([mesh](){
        ImGui::Begin("MyPanel");

        if (ImGui::Button("Load Mesh"))
        {
            // open a file manager to select an ovm file
            load_from_dialog("Select OVM file");
        }
        if(ImGui::Button("Grow"))
        {
            mesh.set_scale(mesh.get_scale() + 0.2f);
        }
        ImGui::SameLine();
        if(ImGui::Button("Shrink"))
        {
            mesh.set_scale(mesh.get_scale() - 0.2f);
        }

        ImGui::End();
    });


    open();
}

