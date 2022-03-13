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
    window->set_custom_imgui([this] {color_splash_ui();});

    color_splash_init();

    // Threaded Mode
    //std::thread* s_run_thread = new std::thread(&Demo::code_demo, this);
    //s_run_thread->join();
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
        window->remove_mesh(m_nut_mesh);

        window->set_custom_imgui([this] {color_splash_ui();});

        color_splash_init();
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
        window->set_face_color(m_nut_mesh, random, Color(1,0,0,1));
        window->set_face_color(m_nut_mesh, prev_random, Color(0,0,0,0));
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

void Demo::color_splash_init()
{
    // Load Example Mesh
    // OVM Setup
    OpenVolumeMesh::GeometricPolyhedralMeshV3d m_mesh;

    OpenVolumeMesh::IO::FileManager file_manager;
    file_manager.readFile("../res/sample_meshes/nut_el0_5_hex_opt.ovm", m_mesh);

    // Add Mesh
    m_hand_mesh = window->add_mesh(&m_mesh);

    // Set Selection Callback
    window->set_callback_face_selection(std::bind(&Demo::color_splash_hit, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    window->camera_focus_on(0,0,0, 0,0,10);

}

void Demo::color_splash_ui()
{
    ImGui::Begin("Custom UI");
    ImGui::SliderFloat("SplashSize", &m_splash_size, 0, 10.0f);
    ImGui::End();
}

void Demo::color_splash_hit(int mesh_id, int element_id, bool selected)
{

    Color splash_color = Color((rand()%1000)/1000.0f, (rand()%1000)/1000.0f, (rand()%1000)/1000.0f, 1.0f);

    std::cout << "Selection Callback: " << mesh_id << " " << element_id << " " << selected << std::endl;
    //std::cout << "Color: " << splash_color.r << " " << splash_color.g << " " << splash_color.b << std::endl;
    if(selected)
    {
        auto mesh_object = window->get_mesh_obj(mesh_id);
        auto ovm_mesh = mesh_object->m_mesh;

        // Calculate Unit triangle size from object bounding box
        m_mesh_scalar_value = (std::abs(mesh_object->get_max().x - mesh_object->get_min().x)
                               + std::abs(mesh_object->get_max().y - mesh_object->get_min().y)
                               + std::abs(mesh_object->get_max().z - mesh_object->get_min().z))/3.0f;
        //std::cout << m_splash_size << " " << m_mesh_scalar_value << " " <<  std::abs(mesh_object->get_max().x - mesh_object->get_min().x) << std::endl;
        //std::cout << VecUtil::to_string(mesh_object->get_max()) << " " << VecUtil::to_string(mesh_object->get_min()) << std::endl;

        auto main_face_iter = ovm_mesh->faces().first;
        main_face_iter += element_id;

        int main_halfface_id = ovm_mesh->face_halffaces(*main_face_iter)[0].idx();
        auto center_point = mesh_object->get_mvb()->get_halfface_barycenter(main_halfface_id);

        // Go through all triangles
        // If their distance is near enough, then chances increase that they get colored
        for(auto face_iter : ovm_mesh->halffaces())
        {
            int id = face_iter.idx();
            auto barycenter = mesh_object->get_mvb()->get_halfface_barycenter(id);

            auto distance = glm::length(barycenter - center_point);

            float chance = (m_splash_size* m_mesh_scalar_value - distance);
            std::cout << distance << " chance: " << chance << std::endl;
            if(chance < 0)
                chance = 0;

            bool do_color = (rand()%100)/100.0f < chance;
            if(do_color)
            {
                int face_id = OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3d>::face_handle(face_iter).idx();
                window->set_face_color(mesh_id, face_id, splash_color);
            }
        }
    }
}