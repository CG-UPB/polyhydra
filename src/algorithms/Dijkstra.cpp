
#include "Dijkstra.h"
#include "random"
#include "iostream"
#include "bits/stdc++.h"

typedef std::pair<int, OpenVolumeMesh::VertexHandle> Node;

namespace vOS
{

    Dijkstra::Dijkstra(): m_weights(&m_mesh, "")
    {
        m_weights = m_mesh.request_edge_property<int>("Weight");
        m_weights->set_persistent(true);

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

        // assign random weights to each edge
        for(OpenVolumeMesh::EdgeIter e_it = m_mesh.edges_begin(); e_it != m_mesh.edges_end(); ++e_it) {
            m_weights[*e_it] = std::rand() % 101 + 1;
        }

        m_start = v0;
        m_end = v3;
    }

    void Dijkstra::init(OpenVolumeMesh::VertexHandle start, OpenVolumeMesh::VertexHandle end)
    {
        m_start = start;
        m_end = end;
    }

    void Dijkstra::run()
    {

        Node currentVertex = std::make_pair(0, m_start);
        std::priority_queue<Node, std::vector<Node>, std::greater<>> queue;
        std::vector<int> distances(m_mesh.n_vertices(), std::numeric_limits<int>::max());

        queue.push(currentVertex);
        distances[currentVertex.second.idx()] = 0;

        bool found = false;
        while (!found && !queue.empty())
        {
            auto vertexHandle = queue.top().second;
            queue.pop();

            for (auto edgeHandle : m_mesh.vertex_edges(vertexHandle))
            {
                auto edgeVertices = m_mesh.edge_vertices(edgeHandle);
                OpenVolumeMesh::VertexHandle nextVertexHandle;
                if (edgeVertices[0].idx() == vertexHandle.idx())
                {
                    nextVertexHandle = edgeVertices[1];
                }
                else
                {
                    nextVertexHandle = edgeVertices[0];
                }

                int distToNext = m_weights[edgeHandle];
                if (distances[nextVertexHandle.idx()] > distances[vertexHandle.idx()] + distToNext)
                {
                    distances[nextVertexHandle.idx()] = distances[vertexHandle.idx()] + distToNext;
                    queue.push(std::make_pair(distances[nextVertexHandle.idx()], nextVertexHandle));
                }

                if (queue.top().second.idx() == m_end.idx())
                {
                    found = true;
                    break;
                }
            }
        }

        std::cout << "Shortest path: " << std::endl;
        while (!queue.empty())
        {
            auto vertex = queue.top();
            queue.pop();
            std::cout << "Vertex: " << vertex.second.idx() << ", weight: " << vertex.first << std::endl;
        }
    }

    void Dijkstra::step()
    {

    }
}