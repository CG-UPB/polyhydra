
#include "Dijkstra.h"
#include "random"
#include "iostream"
#include "queue"
#include "../Window.h"
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

    void Dijkstra::pause_button_pressed()
    {
        LogWindow::getInstance()->addLog("Paused");
        m_pause = true;
    }

    void Dijkstra::pause_button_released()
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

    void Dijkstra::debugging_template_ui_linear()
    {

        static int mesh_count = 0;

        ImGui::Begin("Custom UI");
        // Pause Button
        if (m_pause_toggled)
        {
            // Pause button is active, pressing it would undo pause

            if (ImGui::Button(">"))
            {
                m_pause_toggled = false;
                pause_button_released();
            }
        } else
        {
            // Pause button is inactive, pressing it would pause

            if (ImGui::Button("||"))
            {
                m_pause_toggled = true;
                pause_button_pressed();
            }
        }
        // Reset Button
        if (ImGui::Button("Reset"))
        {
            reset_button_pressed();
        }

        // Step Button
        if (ImGui::Button("Step"))
        {
            step_button_pressed();
        }

        if (ImGui::Button("Open File"))
        {
            m_open_file = true;
        }

        if (m_open_file)
        {
            std::string path;
            if (Window::instance().ShowFileDialog(path))
            {
                OpenVolumeMesh::IO::FileManager file_manager;
                file_manager.readFile(path, m_mesh);
                Window::instance().set_mesh(&m_mesh, mesh_count++);

                linear_run();
                m_open_file = false;
            }
        }

        ImGui::End();
    }

    void Dijkstra::start()
    {
        bool linear = true;

        if(linear)
        {
            // Run Dijkstra linearly
            Window::instance().set_custom_imgui(std::bind(&Dijkstra::debugging_template_ui_linear, this));
            Window::instance().run();
        }else{
            // Run Dijkstra parallel
            Window::instance().set_custom_imgui(std::bind(&Dijkstra::debugging_template_ui_parallel, this));
            std::cout << " Parallel approach " << std::endl;
            std::thread* vos_thread = new std::thread(&Window::run, &Window::instance());

            parallel_run();

            vos_thread->join();
        }
    }

    void Dijkstra::linear_run()
    {

        /* One Time only Setup */
        Window& window = Window::instance();

        LogWindow::getInstance()->addLog("Start Dijkstra");
        window.get_mesh_obj()->remove_highlights();
        window.remove_all_vertex_highlights();
        init();

        Node currentVertex = std::make_pair(0.0f, m_start);
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> queue;
        std::vector<float> distances(m_mesh.n_vertices(), std::numeric_limits<float>::max());
        std::vector<int> prev(m_mesh.n_vertices(), -1);

        queue.push(currentVertex);
        distances[currentVertex.second.idx()] = 0.0f;

        //window.highlight_vertex(m_start, true, 0, 0, 1, 1);
        //window.highlight_vertex(m_end, true, 0, 0, 1, 1);

        bool found = false;

        auto* box_start = new vOS::Box(0.05f, 0.05f, 0.05f);
        box_start->set_position(m_mesh.vertex(m_start)[0], m_mesh.vertex(m_start)[1], m_mesh.vertex(m_start)[2]);
        box_start->set_base_color(0.2f, 0.2f, 1.0f);

        auto* box_end = new vOS::Box(0.05f, 0.05f, 0.05f);
        box_end->set_position(m_mesh.vertex(m_end)[0], m_mesh.vertex(m_end)[1], m_mesh.vertex(m_end)[2]);
        box_end->set_base_color(0.2f, 0.2f, 1.0f);

        window.add_shape(box_start);
        window.add_shape(box_end);

        while (!found && !queue.empty() && !m_reset)
        {
            if (!m_pause || (m_pause && m_step))
            {

                m_step = false;
                auto vertexHandle = queue.top().second;
                queue.pop();

                window.remove_vertex_highlight(OpenVolumeMesh::VertexHandle(prev[vertexHandle.idx()]));

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
                        //window.highlight_vertex(vertexHandle, 1, 0, 0, 1);
                    }

                    if (queue.top().second.idx() == m_end.idx())
                    {
                        found = true;
                        break;
                    }
                }
            }
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

            window.remove_all_vertex_highlights();

            bool first = true;
            for (int i = 0; i < res.size(); i++)
            {
                auto vertex = OpenVolumeMesh::VertexHandle(res[i]);

                auto* box = new vOS::Box(0.05f, 0.05f, 0.05f);
                box->set_position(m_mesh.vertex(vertex)[0], m_mesh.vertex(vertex)[1],m_mesh.vertex(vertex)[2]);
                if (i == res.size() - 1 || first)
                {
                    //window.highlight_vertex(vertex, true, 0, 0, 1, 1);
                    box->set_base_color(0.2f, 0.2f, 1.0f);
                } else
                {
                    //window.highlight_vertex(vertex, true, 1, 0, 0, 1);
                    box->set_base_color(1.0f, 0.2f, 0.2f);
                }
                window.add_shape(box);
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
    }

    void Dijkstra::debugging_template_ui_parallel()
    {

        ImGui::Begin("Custom UI");
        // Pause Button
        if (m_pause_toggled)
        {
            // Pause button is active, pressing it would undo pause

            if (ImGui::Button(">"))
            {
                m_pause_toggled = false;
                pause_button_released();
            }
        } else
        {
            // Pause button is inactive, pressing it would pause

            if (ImGui::Button("||"))
            {
                m_pause_toggled = true;
                pause_button_pressed();
            }
        }
        // Reset Button
        if (ImGui::Button("Reset"))
        {
            reset_button_pressed();
        }

        // Step Button
        if (ImGui::Button("Step"))
        {
            step_button_pressed();
        }

        if (ImGui::Button("Open File"))
        {
            m_open_file = true;
        }

        if (m_open_file)
        {
            std::string path;
            if (Window::instance().ShowFileDialog(path))
            {

                m_open_file = false;
            }
        }

        ImGui::End();
    }

    void Dijkstra::parallel_run()
    {
        static std::string empty;

        /* One Time only Setup */
        Window& window = Window::instance();

        // Read file
        while(window.get_loaded_file_name() == empty){}
        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile(window.get_loaded_file_name(), m_mesh);
        Window::instance().set_mesh(&m_mesh);
        std::cout << window.get_loaded_file_name() << std::endl;

        LogWindow::getInstance()->addLog("Start Dijkstra");
        window.remove_all_vertex_highlights();
        init();

        Node currentVertex = std::make_pair(0.0f, m_start);
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> queue;
        std::vector<float> distances(m_mesh.n_vertices(), std::numeric_limits<float>::max());
        std::vector<int> prev(m_mesh.n_vertices(), -1);

        queue.push(currentVertex);
        distances[currentVertex.second.idx()] = 0.0f;

        //window.highlight_vertex(m_start, true, 0, 0, 1, 1);
        //window.highlight_vertex(m_end, true, 0, 0, 1, 1);

        bool found = false;

        // Wait for Vos to initialize
        window.is_ready();

        auto* box_start = new vOS::Box(0.05f, 0.05f, 0.05f);
        box_start->set_position(m_mesh.vertex(m_start)[0], m_mesh.vertex(m_start)[1], m_mesh.vertex(m_start)[2]);
        box_start->set_base_color(0.2f, 0.2f, 1.0f);

        auto* box_end = new vOS::Box(0.05f, 0.05f, 0.05f);
        box_end->set_position(m_mesh.vertex(m_end)[0], m_mesh.vertex(m_end)[1], m_mesh.vertex(m_end)[2]);
        box_end->set_base_color(0.2f, 0.2f, 1.0f);

        window.add_shape(box_start);
        window.add_shape(box_end);


        while (!found && !queue.empty() && !m_reset)
        {
            if (!m_pause || (m_pause && m_step))
            {

                m_step = false;
                auto vertexHandle = queue.top().second;
                queue.pop();

                window.remove_vertex_highlight(OpenVolumeMesh::VertexHandle(prev[vertexHandle.idx()]));

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
                        //window.highlight_vertex(vertexHandle, 1, 0, 0, 1);
                    }

                    if (queue.top().second.idx() == m_end.idx())
                    {
                        found = true;
                        break;
                    }
                }
            }
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

            window.get_mesh_obj()->remove_highlights();

            bool first = true;
            for (int i = 0; i < res.size(); i++)
            {
                auto vertex = OpenVolumeMesh::VertexHandle(res[i]);

                auto* box = new vOS::Box(0.05f, 0.05f, 0.05f);
                box->set_position(m_mesh.vertex(vertex)[0], m_mesh.vertex(vertex)[1],m_mesh.vertex(vertex)[2]);
                if (i == res.size() - 1 || first)
                {
                    //window.highlight_vertex(vertex, true, 0, 0, 1, 1);
                    box->set_base_color(0.2f, 0.2f, 1.0f);
                } else
                {
                    //window.highlight_vertex(vertex, true, 1, 0, 0, 1);
                    box->set_base_color(1.0f, 0.2f, 0.2f);
                }
                window.add_shape(box);
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
    }
    void Dijkstra::step()
    {

    }
}