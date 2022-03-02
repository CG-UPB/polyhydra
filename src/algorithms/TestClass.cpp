//
// Created by projektgruppe on 01.03.22.
//

#include "TestClass.h"
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "../Window.h"

namespace vOS {


    void TestClass::initialize() {
        // Set Custom UI for phase changing
        Window::instance().set_vos_initialized(std::bind(&TestClass::start, this));

        Window::instance().open();
    }

    void TestClass::start() {
        // OVM Setup
        OpenVolumeMesh::GeometricPolyhedralMeshV3d myMesh;

        // Create mesh object
// Add eight vertices
        OpenVolumeMesh::VertexHandle v0 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, -1.0, -1.0));
        OpenVolumeMesh::VertexHandle v1 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, -1.0, -1.0));
        OpenVolumeMesh::VertexHandle v2 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, 1.0, -1.0));
        OpenVolumeMesh::VertexHandle v3 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, 1.0, -1.0));
        OpenVolumeMesh::VertexHandle v4 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, -1.0, 1.0));
        OpenVolumeMesh::VertexHandle v5 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, -1.0, 1.0));
        OpenVolumeMesh::VertexHandle v6 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(-1.0, 1.0, 1.0));
        OpenVolumeMesh::VertexHandle v7 = myMesh.add_vertex(OpenVolumeMesh::Vec3d(1.0, 1.0, 1.0));
        std::vector<OpenVolumeMesh::VertexHandle> vertices;
// Add faces
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v3);
        vertices.push_back(v2);
        OpenVolumeMesh::FaceHandle f0 = myMesh.add_face(vertices);
        vertices.clear();
        vertices.push_back(v1);
        vertices.push_back(v5);
        vertices.push_back(v7);
        vertices.push_back(v3);
        OpenVolumeMesh::FaceHandle f1 = myMesh.add_face(vertices);
        vertices.clear();
        vertices.push_back(v5);
        vertices.push_back(v4);
        vertices.push_back(v6);
        vertices.push_back(v7);
        OpenVolumeMesh::FaceHandle f2 = myMesh.add_face(vertices);
        vertices.clear();
        vertices.push_back(v4);
        vertices.push_back(v0);
        vertices.push_back(v2);
        vertices.push_back(v6);
        OpenVolumeMesh::FaceHandle f3 = myMesh.add_face(vertices);
        vertices.clear();
        vertices.push_back(v0);
        vertices.push_back(v4);
        vertices.push_back(v5);
        vertices.push_back(v1);
        OpenVolumeMesh::FaceHandle f4 = myMesh.add_face(vertices);
        vertices.clear();
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v7);
        vertices.push_back(v6);
        OpenVolumeMesh::FaceHandle f5 = myMesh.add_face(vertices);
        std::vector<OpenVolumeMesh::HalfFaceHandle> halffaces;
// Add first tetrahedron
        halffaces.push_back(myMesh.halfface_handle(f0, 0));
        halffaces.push_back(myMesh.halfface_handle(f1, 0));
        halffaces.push_back(myMesh.halfface_handle(f2, 0));
        halffaces.push_back(myMesh.halfface_handle(f3, 0));
        halffaces.push_back(myMesh.halfface_handle(f4, 0));
        halffaces.push_back(myMesh.halfface_handle(f5, 0));
        myMesh.add_cell(halffaces);

        // VOS Window
        Window &window = Window::instance();
        window.add_mesh(&myMesh);

        window.load_mesh_data(0);
    }


    void TestClass::startQuad() {

        std::vector<OpenVolumeMesh::Vec3d> positions;

        positions.emplace_back(-1.0, -1.0, 0.0);
        positions.emplace_back(-1.0, 1.0, 0.0);
        positions.emplace_back(1.0, -1.0, 0.0);
        positions.emplace_back(1.0, 1.0, 0.0);



        // OVM Setup
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

        // VOS Window
        Window &window = Window::instance();
        window.add_mesh(&mesh);

        window.load_mesh_data(0);
    }
}