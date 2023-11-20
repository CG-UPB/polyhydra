#include "volumeshOS.h"
#include <bits/stdc++.h>

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

VMesh setup_mesh(const OpenVolumeMesh::GeometricPolyhedralMeshV3d &ovm_mesh, const std::string &name = "",
                 glm::vec3 position = {0.0, 0.0, 0.0})
{
    // Note: None of these settings needs to be set before opening the viewer
    //       Most of these examples just set the default value for demonstration
    //       See volumeshOS.h for further information

    auto mesh = load(&ovm_mesh);
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
    //mesh.set_rotation(...);

    // if using base color set color as follows
    mesh.set_color(std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f});

    // otherwise set color for each cell
    for (auto cit: ovm_mesh.cells())
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
    mesh.set_lighting_mode(LightingMode::PBR);

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
    //mesh.set_cell_rounding(0.0f);

    // set tessellation level (1-64) for bezier meshes:
    mesh.set_tessellation_level(1);
    mesh.set_visibility(true);

    return mesh;
}

void setup_graphics()
{
    // Do further settings in the viewer

    // Shadows
    use_shadows(true);
    set_shadow_penumbra(10.0f);

    // Ambient Occlusion
    use_ambient_occlusion(true);

    // Transparency
    use_transparency(false);

    // set_rendering_mode(RenderingMode::LINES);

    // Post Processing
    set_gamma(1.4);
    set_saturation(1.0);
    set_contrast(1.0);

    // Chose between ORBIT and FLY for camera
    set_camera_mode(CameraMode::ORBIT);

    // Set direction for light as cube position [-1,1]x[-1,1]x[-1,1]
    set_light_direction(std::array<float, 3>{0.5f, 1.0f, 1.0f});


    // Ground
    // you can choose between a solid ground, a grid or both
    use_grid(true);
    use_ground(true);
    set_ground_height(-5.0f);

}

void face_select(const VMesh mesh, OpenVolumeMesh::FaceHandle fh)
{
    log("Face " + std::to_string(fh.uidx()) + " was selected");
}

void cell_select(const VMesh mesh, OpenVolumeMesh::CellHandle ch)
{
    log("Cell " + std::to_string(ch.uidx()) + " was selected");

    auto color = mesh.get_color<glm::vec4>(ch);
    color = glm::vec4(color.b, color.r, color.g, 1.0f);
    mesh.set_color(ch, color);
}

int main(int argc, char *argv[])
{
    set_theme(Theme::Dark);

    // graphic settings
    setup_graphics();


    // generate ovm mesh
    auto ovm_mesh = gen_mesh();
    // load mesh into view; get VMesh object in return to operate on
    auto mesh = setup_mesh(ovm_mesh, "Cube");
    //auto mesh = load_from_dialog("");

//    auto ovm_mesh2 = std::make_shared<OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>>();
//    OpenVolumeMesh::IO::FileManager file_manager;
//    std::string path = "/home/lukas/CLionProjects/volumeshos/res/OVM/Tet/vase-lion13536.1.ovm";
//    file_manager.readFile(path, *ovm_mesh2);

//    auto mesh = load(path);
//    mesh.set_name("mesh");
//
//    mesh.set_color(std::array<float, 4>{0.7f, 0.15f, 0.15f, 0.5f});
//    mesh.set_metallic(0.3f);
//    mesh.set_roughness(0.5f);

    //auto cylinder = mesh.add_shape<VCylinder>();
    //auto cone = mesh.add_shape<VCone>();
    //auto sphere = mesh.add_shape<VSphere>();
    //auto box = mesh.add_shape<VBox>();
//    auto arrow = mesh.add_shape<VArrow>();
//    arrow.set_scale(std::array<float,3>{0.5f, 1.5f, 0.5f});
//    arrow.set_tip_height(0.35f);
//    arrow.set_base_width(0.5f);
    //cylinder.set_color(std::array<float, 4>{0.7f, 0.15f, 0.15f, 0.5f});


//    for(auto c_it : ovm_mesh2->cells())
//    {
//        for(auto e_it : ovm_mesh2->cell_edges(c_it))
//        {
//            // get both vertices of the edge
//            auto edge = ovm_mesh2->edge(e_it);
//            auto from = ovm_mesh2->vertex(edge.from_vertex());
//            auto to = ovm_mesh2->vertex(edge.to_vertex());
//
//            // calculate direction
//            auto dir = to - from;
//            auto pos = from + (dir / 2.0f);
//
//            // add cylinder
//            auto cylinder = mesh.add_shape<VCylinder>(c_it);
//            cylinder.set_position(pos);
//            float thickness = 0.001f;
//            cylinder.set_scale(thickness, glm::length(glm::vec3(dir[0], dir[1], dir[2])), thickness);
//            cylinder.set_direction(dir);
//        }
//    }



    // define lambda function that gets executed each frame
    // use ImGUI to setup your own GUI

    bool animation = false;
    clock_t animation_start, animation_end;
    int animation_time = 8;
    int image = 0;
    bool render_video = false;
    char name[50]{};

    float x = 0.0f;
    char st[20]{};

    on_gui_render(
            [mesh, &name, &x, &st]() {
                ImGui::Begin("MyPanel");
                if (ImGui::Button("Load Mesh"))
                {
                    // open a file manager to select an ovm file
                    auto m = load_from_dialog("Select OVM file");
                }


//                if (ImGui::Button("Animate "))
//                {
//                    animation = true;
//                    animation_start = clock();
//                    animation_end = animation_start + animation_time * CLOCKS_PER_SEC;
//                    image = 0;
//                }
//        if(!animation && ImGui::SliderInt("Seconds", &animation_time, 0, 20));
//
//        if(animation)
//        {
//            auto current_time = clock();
//            double elapsed_time = (double)(current_time - animation_start) / CLOCKS_PER_SEC;
//
//            double percentage = (elapsed_time / animation_time) * 10.0;
//
//            int max_images = animation_time * 30;
//
//            // percentage = (double)image / (double)max_images;
//
//
//            auto p = std::clamp(sin(PI*(float)percentage) * sin(PI*(float)percentage), 0.0f, 1.0f);
//
////            mesh.set_cell_rounding(p);
////            mesh2.set_cell_size(1.0f - p);
////            mesh3.set_slice_factor(p);
//
//            glm::vec3 dest = glm::vec3{5.0f, 0.0f, 0.0f};
//
//            mesh.set_position(dest * (float)percentage);
//
//            auto pos2 = mesh2.get_position<glm::vec3>();
//            mesh2.set_position(dest * -(float)percentage);
//
//            if(render_video)
//            {
//                ExportOptions options;
//                options.width = 1024;
//                options.height = 768;
//                options.include_background = true;
//                options.include_shapes = true;
//                options.include_ground = true;
//                options.ground_shadow_only = false;
//
//                std::string path = "/home/lukas/CLionProjects/volumeshos/res/readme_res/video_rounding/image";
//                path += std::to_string(image++);
//                path += ".png";
//                export_image(path, options);
//            }
//
//            if (percentage >= 1.0f)
//            {
//                animation = false;
//            }
//
//        }
//


                std::string base_path = "/home/lukas/CLionProjects/volumeshos/res/readme_res/";

                if(ImGui::InputText("Name", name, 50))
                {

                }

                if (ImGui::Button("Screenshot "))
                {
                    ExportOptions options;
                    options.width = 1280;
                    options.height = 720;
                    options.include_background = true;
                    options.include_shapes = true;
                    options.include_ground = true;
                    options.ground_shadow_only = false;

                    std::string s(name);
                    base_path += s;
                    base_path += ".png";
                    export_image(base_path, options);
                }

                ImGui::End();
            });


    on_face_select(&face_select);
    on_cell_select(&cell_select);


    open();
}

