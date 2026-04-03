#include <polyhydra/polyhydra.h>
#include "bits/stdc++.h"

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

polyhydra::VMesh setup_mesh(const OpenVolumeMesh::GeometricPolyhedralMeshV3d& ovm_mesh,
                            const std::string& name = "",
                            glm::vec3 position = {0.0, 0.0, 0.0})
{
    // Note: None of these settings needs to be set before opening the viewer
    //       Most of these examples just set the default value for demonstration
    //       See polyhydra.h for further information

    auto mesh = polyhydra::load(&ovm_mesh);
    mesh.set_name(name);

    // these functions can be used to get insight of cells
    mesh.use_backface_culling(true);
    mesh.use_two_sided_lighting(false);

    // if true there is one color that can be set inside the viewer.
    // otherwise each cell/face has an own color value
    mesh.use_base_color(true);

    // example usage of mesh related functions
    mesh.set_position(position);
    mesh.set_scale(1.0f);
    mesh.use_scale_normalization(false);
    // mesh.set_rotation(...);

    // if using base color set color as follows
    mesh.set_color(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f});

    // otherwise set color for each cell
    for (auto cit : ovm_mesh.cells())
    {
        mesh.set_color(cit, std::array<float, 4>{0.0f, 1.0f, 0.0f, 0.5f});
        mesh.set_cell_size(0.97f);
    }

    // settings for phong lighting
    mesh.set_ambient(1.0f);
    mesh.set_diffuse(1.0f);
    mesh.set_specular(0.15f);
    mesh.set_specular_coefficient(8.0f);

    // settings for physically based lighting (PBR)
    mesh.set_metallic(0.15f);
    mesh.set_roughness(0.65f);

    // set lighting mode: Phong or PBR
    mesh.set_lighting_mode(polyhydra::LightingMode::PBR);

    // slice into the mesh; direction is given by view direction
    mesh.set_slice_factor(0.0f);
    mesh.set_slice_locked(false);
    // peel the mesh; opens inner layers;
    mesh.set_peel_level(0.0f);
    // when true peeling is done from the inside to the outside
    mesh.use_reverse_peeling(false);
    // set cell size; moves cell vertices to its center
    mesh.set_cell_size(1.0f);
    // rounds the edges of a cell
    // mesh.set_cell_rounding(0.0f);

    // set tessellation level (1-64) for bezier meshes:
    mesh.set_tessellation_level(1);
    mesh.set_visibility(true);

    return mesh;
}

void setup_graphics()
{
    // Do further settings in the viewer

    // Shadows
    polyhydra::use_shadows(true);
    polyhydra::set_shadow_penumbra(10.0f);

    // Ambient Occlusion
    polyhydra::use_ambient_occlusion(true);

    // Transparency
    polyhydra::use_transparency(false);

    // set_rendering_mode(RenderingMode::LINES);

    // Post Processing
    polyhydra::set_gamma(1.4);
    polyhydra::set_saturation(1.0);
    polyhydra::set_contrast(1.0);

    // Chose between ORBIT and FLY for camera
    polyhydra::set_camera_mode(polyhydra::CameraMode::ORBIT);

    // Set direction for light as cube position [-1,1]x[-1,1]x[-1,1]
    polyhydra::set_light_direction(std::array<float, 3>{0.5f, 1.0f, 1.0f});

    // Ground
    // you can choose between a solid ground, a grid or both
    polyhydra::use_grid(true);
    polyhydra::use_ground(true);
    polyhydra::set_ground_height(-5.0f);
}

void face_select(const polyhydra::VMesh mesh, OpenVolumeMesh::FaceHandle fh)
{
    polyhydra::log("Face " + std::to_string(fh.uidx()) + " was selected");
}

void cell_select(const polyhydra::VMesh mesh, OpenVolumeMesh::CellHandle ch)
{
    polyhydra::log("Cell " + std::to_string(ch.uidx()) + " was selected");

    auto color = mesh.get_color<glm::vec4>(ch);
    color = glm::vec4(color.b, color.r, color.g, 1.0f);
    mesh.set_color(ch, color);
}

int main(int argc, char* argv[])
{
    polyhydra::set_theme(polyhydra::Theme::Dark);

    // graphic settings
    setup_graphics();

    auto mesh = gen_mesh();

    polyhydra::load(&mesh, "Single-cube Mesh");

    polyhydra::on_gui_render(
        []()
        {
            ImGui::Begin("MyPanel");
            if (ImGui::Button("Load Mesh"))
            {
                // open a file manager to select an ovm file
                auto mesh = polyhydra::load_from_dialog("Select OVM file");
            }
            ImGui::End();
        });

    polyhydra::open();
}
