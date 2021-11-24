
#include "Dijkstra.h"
#include "random"
#include "iostream"
#include "bits/stdc++.h"
#include "../VosWindow.h"
#include <chrono>
#include <thread>
#include <math.h>
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include "ImGuiFileDialog.h"

typedef std::pair<float, OpenVolumeMesh::VertexHandle> Node;

namespace vOS
{

    Dijkstra::Dijkstra() : m_weights(&m_mesh, "")
    {
        m_weights = m_mesh.request_edge_property<float>("Weight");
        m_weights->set_persistent(true);

    }

    void Dijkstra::init()
    {

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

        for (OpenVolumeMesh::VertexIter v_it = m_mesh.vertices_begin();
             v_it != m_mesh.vertices_end(); ++v_it)
        {
            if (v_it->idx() == start)
            {
                m_start = *v_it;
                std::cout << "start: " << start << std::endl;
            } else if (v_it->idx() == end)
            {
                m_end = *v_it;
                std::cout << "end: " << end << std::endl;;
            }
        }
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

    void Dijkstra::reset_button_pressed()
    {
        m_reset = true;
    }

    void Dijkstra::debugging_template_ui()
    {

        ImGui::Begin("Custom UI");
        // Pause Button
        if (m_pause_toggled)
        {
            // Pause button is active, pressing it would undo pause

            if (ImGui::Button(">"))
            {
                m_pause_toggled = false;
                VosWindow::instance().m_on_vos_unpaused();
            }
        } else
        {
            // Pause button is inactive, pressing it would pause

            if (ImGui::Button("||"))
            {
                m_pause_toggled = true;
                VosWindow::instance().m_on_vos_paused();
            }
        }
        // Reset Button
        if (ImGui::Button("Reset"))
        {
            VosWindow::instance().m_on_reset();
        }

        // Step Button
        if (ImGui::Button("Step"))
        {
            VosWindow::instance().m_on_step();
        }

        if (ImGui::Button("Open File"))
        {
            m_open_file = true;
        }

        if (m_open_file)
        {
            std::string path;
            if (VosWindow::ShowFileDialog(path))
            {
                OpenVolumeMesh::IO::FileManager file_manager;
                file_manager.readFile(path, m_mesh);
                VosWindow::instance().set_mesh(&m_mesh);
                std::cout << path << std::endl;
                run();
                m_open_file = false;
            }
        }

        ImGui::End();
    }

    void Dijkstra::init_vos()
    {
        VosWindow& window = VosWindow::instance();
        window.set_callback_paused(std::bind(&Dijkstra::PauseButtonPressed, this));
        window.set_callback_unpaused(std::bind(&Dijkstra::PauseButtonReleased, this));
        window.set_callback_step(std::bind(&Dijkstra::step_button_pressed, this));
        window.set_callback_reset(std::bind(&Dijkstra::reset_button_pressed, this));

        window.set_custom_imgui(std::bind( &Dijkstra::debugging_template_ui, this));

        window.open();
        window.render_loop();
    }

    void Dijkstra::run()
    {

        /* One Time only Setup */
        VosWindow& window = VosWindow::instance();

        LogWindow::getInstance()->addLog("Start Dijkstra");
        window.get_mesh_obj().remove_highlights();
        init();

        Node currentVertex = std::make_pair(0.0f, m_start);
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> queue;
        std::vector<float> distances(m_mesh.n_vertices(), std::numeric_limits<float>::max());
        std::vector<int> prev(m_mesh.n_vertices(), -1);

        queue.push(currentVertex);
        distances[currentVertex.second.idx()] = 0.0f;

        //window.set_vertex_color(m_start, true, 0, 0, 1, 1);
        //window.set_vertex_color(m_end, true, 0, 0, 1, 1);

        bool found = false;

        window.add_box(m_mesh.vertex(m_start)[0], m_mesh.vertex(m_start)[1], m_mesh.vertex(m_start)[2], 0.2f, 0.2f,
                       1.0f);
        window.add_box(m_mesh.vertex(m_end)[0], m_mesh.vertex(m_end)[1], m_mesh.vertex(m_end)[2], 0.2f, 0.2f, 1.0f);

        while (!found && !queue.empty() && !m_reset)
        {
            if (!m_pause || (m_pause && m_step))
            {

                m_step = false;
                auto vertexHandle = queue.top().second;
                queue.pop();

                window.set_vertex_color(OpenVolumeMesh::VertexHandle(prev[vertexHandle.idx()]), false, 1, 0, 0, 1);

                // voh iterator
                for (auto edgeHandle: m_mesh.vertex_edges(vertexHandle))
                {
                    auto edgeVertices = m_mesh.edge_vertices(edgeHandle);
                    OpenVolumeMesh::VertexHandle nextVertexHandle;
                    if (edgeVertices[0].idx() == vertexHandle.idx())
                    {
                        nextVertexHandle = edgeVertices[1];
                    } else
                    {
                        nextVertexHandle = edgeVertices[0];
                    }

                    float distToNext = m_weights[edgeHandle];
                    if (distances[nextVertexHandle.idx()] > distances[vertexHandle.idx()] + distToNext)
                    {
                        distances[nextVertexHandle.idx()] = distances[vertexHandle.idx()] + distToNext;
                        queue.push(std::make_pair(distances[nextVertexHandle.idx()], nextVertexHandle));
                        prev[nextVertexHandle.idx()] = vertexHandle.idx();
                        window.set_vertex_color(vertexHandle, true, 1, 0, 0, 1);
                    }

                    if (queue.top().second.idx() == m_end.idx())
                    {
                        found = true;
                        break;
                    }
                }
            }
            //logic = window.render_manual();
        }

        if (!m_reset)
        {

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

                if (i == res.size() - 1 || first)
                {
                    //window.set_vertex_color(vertex, true, 0, 0, 1, 1);
                    window.add_box(m_mesh.vertex(vertex)[0], m_mesh.vertex(vertex)[1], m_mesh.vertex(vertex)[2],
                                   0.2f, 0.2f, 1.0f);
                } else
                {
                    //window.set_vertex_color(vertex, true, 1, 0, 0, 1);
                    window.add_box(m_mesh.vertex(vertex)[0], m_mesh.vertex(vertex)[1],
                                   m_mesh.vertex(vertex)[2]);
                }
                first = false;

                std::cout << "Vertex: " << vertex.idx() << std::endl;
            }
            LogWindow::getInstance()->addLog("Dijkstra function ended");
            LogWindow::getInstance()->addLog("Continue");

        }
        if (window.is_running())
        {
            LogWindow::getInstance()->addLog("Reset Variables");
            m_reset = false;
            m_step = false;
        }
        std::cout << "End Dijkstra" << std::endl;
        //window.close();
    }

    void Dijkstra::step()
    {

    }
}