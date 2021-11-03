
#include "Dijkstra.h"
#include "random"
#include "iostream"
#include "bits/stdc++.h"
#include "VosWindow.h"
#include <chrono>
#include <thread>
#include <math.h>
#include <OpenVolumeMesh/FileManager/FileManager.hh>

typedef std::pair<float, OpenVolumeMesh::VertexHandle> Node;

namespace vOS
{

    Dijkstra::Dijkstra() : m_weights(&m_mesh, "")
    {
        m_weights = m_mesh.request_edge_property<float>("Weight");
        m_weights->set_persistent(true);

        OpenVolumeMesh::IO::FileManager file_manager;
        //file_manager.readFile("/home/steffen/Downloads/OVM/Tet/bunny5824.1.ovm", m_mesh);

        // assign random weights to each edge
        for (OpenVolumeMesh::EdgeIter e_it = m_mesh.edges_begin(); e_it != m_mesh.edges_end(); ++e_it)
        {
            auto vertices = m_mesh.edge_vertices(*e_it);
            auto first = m_mesh.vertex(vertices[0]);
            auto second = m_mesh.vertex(vertices[1]);
            float dx = second[0] - first[0];
            float dy = second[1] - first[1];
            float dz = second[2] - first[2];
            float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
            m_weights[*e_it] = dist;
        }

        srand(time(nullptr));
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

    void Dijkstra::step_button_pressed()
    {
        m_step = true;
    }

    void Dijkstra::run()
    {

        Node currentVertex = std::make_pair(0.0f, m_start);
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> queue;
        std::vector<float> distances(m_mesh.n_vertices(), std::numeric_limits<float>::max());
        std::vector<int> prev(m_mesh.n_vertices(), -1);

        VosWindow& window = VosWindow::instance();

        window.set_mesh(&m_mesh);

        queue.push(currentVertex);
        distances[currentVertex.second.idx()] = 0.0f;

        window.set_callback_paused(std::bind( &Dijkstra::PauseButtonPressed, this));
        window.set_callback_unpaused(std::bind( &Dijkstra::PauseButtonReleased, this));
        window.set_callback_step(std::bind( &Dijkstra::step_button_pressed, this));

        window.set_vertex_color(m_start, true, 0,0,1,1);
        window.set_vertex_color(m_end, true, 0,0,1,1);

        while(!window.is_ready()){}

        window.Log()->addLog("Press Pause");
        bool found = false;
        while (!found && !queue.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (!m_pause)
            {
                //m_step = false;
                auto vertexHandle = queue.top().second;
                queue.pop();

                window.set_vertex_color(OpenVolumeMesh::VertexHandle(prev[vertexHandle.idx()]), false, 1,0,0,1);

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

                    float distToNext = m_weights[edgeHandle];
                    if (distances[nextVertexHandle.idx()] > distances[vertexHandle.idx()] + distToNext)
                    {
                        distances[nextVertexHandle.idx()] = distances[vertexHandle.idx()] + distToNext;
                        queue.push(std::make_pair(distances[nextVertexHandle.idx()], nextVertexHandle));
                        prev[nextVertexHandle.idx()] = vertexHandle.idx();
                        window.set_vertex_color(vertexHandle, true, 1,0,0,1);
                    }

                    if (queue.top().second.idx() == m_end.idx())
                    {
                        found = true;
                        break;
                    }
                }
            }
        }

        std::vector<int> res;
        int temp = m_end.idx();
        res.push_back(temp);
        while (temp != m_start.idx())
        {
            temp = prev[temp];
            res.push_back(temp);
        }

        window.get_mesh_obj().remove_highlights();

        bool first = true;
        for (int i = 0; i < res.size(); i++)
        {
            auto vertex = OpenVolumeMesh::VertexHandle(res[i]);

            if(i == res.size() - 1 || first)
                window.set_vertex_color(vertex, true, 0,0,1,1);
            else
                window.set_vertex_color(vertex, true, 1,0,0,1);
            first = false;

            std::cout << "Vertex: " << vertex.idx() << std::endl;
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