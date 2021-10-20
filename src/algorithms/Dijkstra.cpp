
#include "Dijkstra.h"

namespace vOS
{

    void Dijkstra::init()
    {

        // Add eight vertices
        OpenVolumeMesh::VertexHandle v0 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f(-1.0, 0.0, 0.0));
        OpenVolumeMesh::VertexHandle v1 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f( 0.0, 0.0, 1.0));
        OpenVolumeMesh::VertexHandle v2 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f( 1.0, 0.0, 0.0));
        OpenVolumeMesh::VertexHandle v3 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f( 0.0, 0.0,-1.0));
        OpenVolumeMesh::VertexHandle v4 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f( 0.0, 1.0, 0.0));

        std::vector<OpenVolumeMesh::VertexHandle> vertices;

        // Add faces
        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v4);
        OpenVolumeMesh::FaceHandle f0 = m_mesh.add_face(vertices);
        vertices.clear();

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v4);
        OpenVolumeMesh::FaceHandle f1 = m_mesh.add_face(vertices);
        vertices.clear();

        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        OpenVolumeMesh::FaceHandle f2 = m_mesh.add_face(vertices);
        vertices.clear();

        vertices.push_back(v0);
        vertices.push_back(v4);
        vertices.push_back(v2);
        OpenVolumeMesh::FaceHandle f3 = m_mesh.add_face(vertices);
        vertices.clear();

        vertices.push_back(v0);
        vertices.push_back(v4);
        vertices.push_back(v3);
        OpenVolumeMesh::FaceHandle f4 = m_mesh.add_face(vertices);
        vertices.clear();

        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);
        OpenVolumeMesh::FaceHandle f5 = m_mesh.add_face(vertices);
        vertices.clear();

        vertices.push_back(v0);
        vertices.push_back(v2);
        vertices.push_back(v3);
        OpenVolumeMesh::FaceHandle f6 = m_mesh.add_face(vertices);

        std::vector<OpenVolumeMesh::HalfFaceHandle> halffaces;

        // Add first tetrahedron
        halffaces.push_back(m_mesh.halfface_handle(f0, 1));
        halffaces.push_back(m_mesh.halfface_handle(f1, 1));
        halffaces.push_back(m_mesh.halfface_handle(f2, 0));
        halffaces.push_back(m_mesh.halfface_handle(f3, 1));
        m_mesh.add_cell(halffaces);

        // Add second tetrahedron
        halffaces.clear();
        halffaces.push_back(m_mesh.halfface_handle(f4, 1));
        halffaces.push_back(m_mesh.halfface_handle(f5, 1));
        halffaces.push_back(m_mesh.halfface_handle(f3, 0));
        halffaces.push_back(m_mesh.halfface_handle(f6, 0));
        m_mesh.add_cell(halffaces);
    }

    void Dijkstra::run()
    {

    }

    void Dijkstra::step()
    {

    }
}