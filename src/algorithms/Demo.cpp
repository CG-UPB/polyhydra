//
// Created by jan on 12.03.22.
//

#include "Demo.h"

#include "../Window.h"
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <functional>
#include <thread>
#include "../util/VecUtil.h"


using namespace vOS;

Demo::Demo()
{
    window = &Window::instance();

    // Set Custom UI for phase changing
    window->set_vos_initialized([this] { start(); });

    window->open();
}

void Demo::start()
{
    // Imgui Callback
    window->set_custom_imgui([this] {code_demo_ui();});

    // Threaded Mode
    std::thread* s_run_thread = new std::thread(&Demo::code_demo, this);
    s_run_thread->join();
}

void Demo::code_demo()
{
    // Load Example Mesh
    // OVM Setup
    OpenVolumeMesh::GeometricPolyhedralMeshV3d m_mesh;

    OpenVolumeMesh::IO::FileManager file_manager;
    file_manager.readFile("../res/sample_meshes/nut_el0_5_hex_opt.ovm", m_mesh);

    m_nut_mesh = window->add_mesh(&m_mesh);

    window->set_mesh_color(Color(0,1,1,1));
}

void Demo::code_demo_ui()
{
    ImGui::Begin("Custom UI");


    // Next Phase
    if (ImGui::Button("Next")) {

    }

    static bool move = true;
    std::string move_message = "Halt Movement";
    if(move)
        move_message = "Halt Movement";
    else
        move_message = "Start Movement";
    // Stop / Start Circle Movememnt
    if (ImGui::Button(move_message.c_str())) {
        move = !move;
    }

    // Focus Random Spot
    if(ImGui::Button("Focus random spot"))
    {
        move = false;
        static int random = 0;
        int prev_random = random;
        random = std::rand()%100;

        window->camera_focus_on(m_nut_mesh, random, 2.5f);
        //window->select_element(m_nut_mesh, random, 0);
        //window->unselect_element(prev_random, random, 0);
        //window->set_face_color(m_nut_mesh, random, Color(1,0,0,1));
        //window->set_face_color(m_nut_mesh, prev_random, Color(0,0,0,0));
    }

    // Orbit Mode
    if(ImGui::Button("Orbit"))
    {
        move = false;

        window->camera_mode(1, 10);
        window->camera_focus_on(0,0,0, 0,0, 10);
    }

    // Move Camera in a circle around Mesh
    static float time = 0;

    if(move)
    {
        time += 0.05f;

        // Set Camera Target to the midpoint of Mesh Object
        auto mesh_position = window->get_focused_mesh_object()->get_data().m_position;

        window->camera_look_at(mesh_position.x, mesh_position.y, mesh_position.z);

        // Set Camera Position along a circle around the Mesh Object
        glm::vec3 cam_position = {cos(time), -sin(time)/2, sin(time)};

        cam_position *= 15.0f;
        cam_position += mesh_position;

        window->camera_set_position(cam_position.x, cam_position.y, cam_position.z);

    }
    ImGui::End();
}