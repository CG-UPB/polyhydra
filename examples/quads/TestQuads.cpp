//
// Created by lukas on 24.02.22.
//

#include "TestQuads.h"
#include "Window.h"
#include <ctime>
#include "OpenVolumeMesh/FileManager/FileManager.hh"
#include "panels/NewFileDialog.h"


TestQuads::TestQuads()
{
    //m_quad_positions = std::vector<OpenVolumeMesh::Vec3d>;
    m_quad_positions.emplace_back(-1.0, -1.0, 0.0);
    m_quad_positions.emplace_back(-1.0, 1.0, 0.0);
    m_quad_positions.emplace_back(1.0, -1.0, 0.0);
    m_quad_positions.emplace_back(1.0, 1.0, 0.0);

    m_ground_positions.emplace_back(-1.0, 0.0, -1.0);
    m_ground_positions.emplace_back(1.0, 0.0, -1.0);
    m_ground_positions.emplace_back(-1.0, 0.0, 1.0);
    m_ground_positions.emplace_back(1.0, 0.0, 1.0);
}

void TestQuads::ui()
{
    vOS::Window &window = vOS::Window::instance();
    ImGui::Begin("Custom UI");
    // Next Phase
    if (ImGui::Button("Add Quad"))
    {
        add_quad();
    }
    if (ImGui::Button("Load Mesh"))
    {
        vOS::NewFileDialog file_dialog;

        char const * filename;

        filename = file_dialog.openDialog("Open Mesh File");

        if (filename != NULL){
            vOS::Window &window = vOS::Window::instance();
            OpenVolumeMesh::GeometricPolyhedralMeshV3d mesh;
            OpenVolumeMesh::IO::FileManager file_manager;
            file_manager.readFile(filename, mesh);
            window.add_mesh(&mesh);
        }
    }
    ImGui::End();
}

OpenVolumeMesh::GeometricPolyhedralMeshV3d TestQuads::create_quad(std::vector<OpenVolumeMesh::Vec3d> positions)
{
    OpenVolumeMesh::GeometricPolyhedralMeshV3d mesh;

    OpenVolumeMesh::VertexHandle v0 = mesh.add_vertex(positions[3]);
    OpenVolumeMesh::VertexHandle v1 = mesh.add_vertex(positions[2]);
    OpenVolumeMesh::VertexHandle v2 = mesh.add_vertex(positions[1]);
    OpenVolumeMesh::VertexHandle v3 = mesh.add_vertex(positions[0]);

    std::vector<OpenVolumeMesh::VertexHandle> vertices;

    vertices.push_back(v2);vertices.push_back(v1);vertices.push_back(v0);
    OpenVolumeMesh::FaceHandle f0 = mesh.add_face(vertices);
    vertices.clear();

    vertices.push_back(v2);vertices.push_back(v3);vertices.push_back(v1);
    OpenVolumeMesh::FaceHandle f1 = mesh.add_face(vertices);
    vertices.clear();

    vertices.push_back(v1);vertices.push_back(v2);vertices.push_back(v0);
    OpenVolumeMesh::FaceHandle f3 = mesh.add_face(vertices);
    vertices.clear();

    vertices.push_back(v3);vertices.push_back(v2);vertices.push_back(v1);
    OpenVolumeMesh::FaceHandle f4 = mesh.add_face(vertices);
    vertices.clear();

    std::vector<OpenVolumeMesh::HalfFaceHandle> half_faces;

    half_faces.push_back(mesh.halfface_handle(f0,1));
    half_faces.push_back(mesh.halfface_handle(f1,1));
    mesh.add_cell(half_faces);

    half_faces.push_back(mesh.halfface_handle(f3,1));
    half_faces.push_back(mesh.halfface_handle(f4,1));
    mesh.add_cell(half_faces);

    return mesh;
}

void TestQuads::add_quad()
{
    vOS::Window &window = vOS::Window::instance();
    auto mesh = create_quad(m_quad_positions);
    int mesh_id = window.add_mesh(&mesh);

    srand(time(nullptr));
    window.set_mesh_color(mesh_id, vOS::Color((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, 0.4));
    //window.set_mesh_color(mesh_id, vOS::Color( 0.0, 0.0, 1.0,1.0));


}

void TestQuads::run()
{
    vOS::Window &window = vOS::Window::instance();

    window.set_custom_imgui(std::bind(&TestQuads::ui, this));

    auto ground = create_quad(m_ground_positions);
    int ground_id = window.add_mesh(&ground);
    window.set_mesh_scale(ground_id, 15);
    window.set_mesh_position(ground_id, 0.0, -10.0, 0.0);
    window.set_mesh_color(ground_id,vOS::Color(0.6f, 0.6f, 0.6f, 1.0f));

    auto wall = create_quad(m_quad_positions);
    int wall_id = window.add_mesh(&wall);
    window.set_mesh_scale(wall_id, 15);
    window.set_mesh_position(wall_id, 0.0, 20.0, -40.0);
    window.set_mesh_color(wall_id,vOS::Color(0.6f, 0.6f, 0.6f, 1.0f));


    float translate = 5.0;
    srand(time(nullptr));
    for(int i = 0; i < 3; i++)
    {
        auto mesh = create_quad(m_quad_positions);
        int mesh_id = window.add_mesh(&mesh);
        window.set_mesh_position(mesh_id, (((float)i)/2)*translate, (((float)i)/2)*translate, ((float)i)*translate);
        //window.set_mesh_color(mesh_id, vOS::Color((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX));
        window.set_mesh_color(mesh_id, vOS::Color( 0.0, 0.0, 1.0,1.0));

    }

    window.open();
}


