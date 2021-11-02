
#include "Dijkstra.h"
#include "random"
#include "iostream"
#include "bits/stdc++.h"
#include "VosWindow.h"
#include <chrono>
#include <thread>
#include <OpenVolumeMesh/FileManager/FileManager.hh>

typedef std::pair<int, OpenVolumeMesh::VertexHandle> Node;

namespace vOS
{

    Dijkstra::Dijkstra() : m_weights(&m_mesh, "")
    {
        m_weights = m_mesh.request_edge_property<int>("Weight");
        m_weights->set_persistent(true);

        // Add eight vertices
        OpenVolumeMesh::VertexHandle v0 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f(-1.0, 0.0, 0.0));
        OpenVolumeMesh::VertexHandle v1 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f(0.0, 0.0, 1.0));
        OpenVolumeMesh::VertexHandle v2 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f(1.0, 0.0, 0.0));
        OpenVolumeMesh::VertexHandle v3 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f(0.0, 0.0, -1.0));
        OpenVolumeMesh::VertexHandle v4 = m_mesh.add_vertex(OpenVolumeMesh::Vec3f(0.0, 1.0, 0.0));

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

        m_start = v0;
        m_end = v0;

        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile("/home/projektgruppe/CLionProjects/volumeshos/bunny5824.1.ovm", m_mesh);


        // assign random weights to each edge
        for (OpenVolumeMesh::EdgeIter e_it = m_mesh.edges_begin(); e_it != m_mesh.edges_end(); ++e_it)
        {
            m_weights[*e_it] = std::rand() % 101 + 1;
        }

        int start = std::rand() % m_mesh.n_vertices();
        int end = std::rand() % m_mesh.n_vertices();

        for(OpenVolumeMesh::VertexIter v_it = m_mesh.vertices_begin();
            v_it != m_mesh.vertices_end(); ++v_it)
        {
            if (v_it->idx() == start)
            {
                m_start = *v_it;
                std::cout << "start: " << start << std::endl;
            }
            else if (v_it->idx() == end)
            {
                m_end = *v_it;
                std::cout << "end: " << end << std::endl;;
            }
        }

    }

    void Dijkstra::init(OpenVolumeMesh::VertexHandle start, OpenVolumeMesh::VertexHandle end)
    {
        m_start = start;
        m_end = end;
    }

    void Dijkstra::PauseButtonPressed()
    {
        LogWindow::getInstance()->addLog("Paused");
        m_pause = true;
    }
    void Dijkstra::PauseButtonReleased()
    {
        LogWindow::getInstance()->addLog("Unpaused");
        m_pause = false;
    }

    void Dijkstra::run()
    {

        Node currentVertex = std::make_pair(0, m_start);
        std::priority_queue<Node, std::vector<Node>, std::greater<>> queue;
        std::vector<int> distances(m_mesh.n_vertices(), std::numeric_limits<int>::max());

        VosWindow& window = VosWindow::instance();

        window.set_mesh(&m_mesh);


        window.set_callback_paused(std::bind( &Dijkstra::PauseButtonPressed, this));
        window.set_callback_unpaused(std::bind( &Dijkstra::PauseButtonReleased, this));

        queue.push(currentVertex);
        distances[currentVertex.second.idx()] = 0;

        while(!window.is_ready()){}

        window.Log()->addLog("Press Pause");
        bool found = false;
        bool next_step_ready = false;
        while (!found && !queue.empty())
        {
            next_step_ready = window.is_ready();
            if (next_step_ready)
            {
                auto vertexHandle = queue.top().second;
                queue.pop();

                // voh iterator
                for (auto edgeHandle: m_mesh.vertex_edges(vertexHandle))
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
            //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        }

        bool first = true;
        while (!queue.empty())
        {
            auto vertex = queue.top();

            if(queue.empty() || first)
                window.set_vertex_color(vertex.second, true, 0,0,1,1);
            else
                window.set_vertex_color(vertex.second, true, 1,0,0,1);
            first = false;

            queue.pop();
            std::cout << "Vertex: " << vertex.second.idx() << ", weight: " << vertex.first << std::endl;
        }

        window.Log()->addLog("Press Pause");
        while(!m_pause);
        window.Log()->addLog("Press Unpause");
        while(m_pause);
        window.Log()->addLog("Dijkstra function ended");
    }

    void Dijkstra::step()
    {

    }
}