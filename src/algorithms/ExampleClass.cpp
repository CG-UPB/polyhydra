//
// Created by jan on 14.01.22.
//

#include "ExampleClass.h"
#include "../Window.h"
#include <OpenVolumeMesh/FileManager/FileManager.hh>
#include <functional>
#include <thread>

using namespace vOS;

void ExampleClass::initialize() {
    // Set Custom UI for phase changing
    Window::instance().set_vos_initialized(std::bind(&ExampleClass::start, this));

    Window::instance().open();
}

void ExampleClass::start()
{
    Window::instance().set_custom_imgui(std::bind(&ExampleClass::selection_demonstration_ui, this));
    std::thread* s_run_thread = new std::thread(&ExampleClass::simple_run, this);
    s_run_thread->join();
}

void ExampleClass::simple_demonstration_ui(){
    ImGui::Begin("Custom UI");
    // Next Phase
    if (ImGui::Button("Next"))
    {
        m_phase++;
        Window::instance().set_custom_imgui(std::bind(&ExampleClass::toolbar_demonstration_ui, this));
        Window::instance().remove_all_meshes();
        toolbar_run();
    }
    ImGui::End();
}

void ExampleClass::simple_run(){

    // OVM Setup
    OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh;

    OpenVolumeMesh::IO::FileManager file_manager;
    file_manager.readFile("../res/sample_meshes/nut_el0_5_hex_opt.ovm", m_mesh);

    // VOS Window
    Window& window = Window::instance();
    window.add_mesh(&m_mesh);

}

void ExampleClass::set_mesh_data()
{
    Window& window = Window::instance();

    // Customize Hand Mesh
    window.set_mesh_cell_size(hand, 0.6f);
    window.set_mesh_color(hand, Color(1,0,0,1));
    window.set_mesh_peel_level(hand, 1);
    window.set_mesh_slice_level(hand, 0.2f);
    // Customize Guy Mesh
    Color hand_color = window.get_mesh_color(hand);
    hand_color.r -= 0.5f;
    hand_color.b += 0.8f;
    window.set_mesh_color(guy, hand_color);
    window.set_mesh_rendering_mode(guy, "mesh_wireframe");
    window.set_mesh_slice_level(guy, 0.5f);
    window.set_mesh_peel_level(guy, 1);
    window.set_mesh_cell_size(guy, 0.6f);

}

void ExampleClass::toolbar_demonstration_ui(){
    ImGui::Begin("Custom UI");
    // Next Phase
    if (ImGui::Button("Next"))
    {
        m_phase++;
        Window::instance().set_custom_imgui(std::bind(&ExampleClass::selection_demonstration_ui, this));
        Window::instance().remove_all_meshes();

        // Selection Run Setup

        // OVM Setup
        OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh_1;

        OpenVolumeMesh::IO::FileManager file_manager;
        file_manager.readFile("../res/sample_meshes/hand4234.1.ovm", m_mesh_1);

        // VOS Window
        Window& window = Window::instance();
        hand =  window.add_mesh(&m_mesh_1);

        selection_run();

    }

    if(ImGui::Button("Reset Changes")){
        set_mesh_data();
    }
    ImGui::End();
}
void ExampleClass::toolbar_run(){
    // OVM Setup
    OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh_1;
    OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh_2;

    OpenVolumeMesh::IO::FileManager file_manager;
    file_manager.readFile("../res/sample_meshes/hand4234.1.ovm", m_mesh_1);

    file_manager.readFile("../res/sample_meshes/guy2500.1.ovm", m_mesh_2);

    // VOS Window
    Window& window = Window::instance();
    hand =  window.add_mesh(&m_mesh_1);
    guy =  window.add_mesh(&m_mesh_2);

    LogWindow::getInstance()->addLog("We add some test logs here", 0);
    LogWindow::getInstance()->addLog("Level 3 Info", 3);

    // Set Variables
    set_mesh_data();
};



void ExampleClass::selection_demonstration_ui(){

    static std::thread* s_run_thread;
    ImGui::Begin("Custom UI");
/*
    if(ImGui::Button("Save Mesh Data"))
    {
        Window::instance().save_mesh_data(0);
    }

    static float spec_strength;
    if (ImGui::SliderFloat("Spec Strength", &spec_strength, 0.0f, 10.0f, "%.2f"))
    {
        Window::instance().set_mesh_specular_strength(0, spec_strength);
    }

    static float spec_exp;
    if (ImGui::SliderFloat("Spec Exponent", &spec_exp, 0.0f, 10.0f, "%.2f"))
    {
        Window::instance().set_mesh_specular_exponent(0, spec_exp);
    }

    static float ambient_strength;
    if (ImGui::SliderFloat("Ambient Strength", &ambient_strength, 0.0f, 10.0f, "%.2f"))
    {
        Window::instance().set_mesh_ambient_strength(0, ambient_strength);
    }

    static float diffuse_strength;
    if (ImGui::SliderFloat("Diffuse Strength", &diffuse_strength, 0.01f, 10.0f, "%.2f"))
    {
        Window::instance().set_mesh_diffuse_strength(0, diffuse_strength);
    }
*/
    // Next Phase
    if (ImGui::Button("Next"))
    {
        m_phase++;

        if(s_run_thread != nullptr)
            s_run_thread->join();
        Window::instance().set_custom_imgui(std::bind(&ExampleClass::bounding_demonstration_ui, this));
        Window::instance().remove_all_shapes();
        Window::instance().remove_all_meshes();

        bounding_run();
    }

    // Selection Slider
    int pre_level = selection_level;
    int pre_type = selection_type;
    ImGui::SliderInt("Level", &selection_level, 0, 10000);
    ImGui::SliderInt("Type", &selection_type, 0, 3);

    if(pre_level != selection_level || pre_type != selection_type) {
        if(s_run_thread != nullptr)
            s_run_thread->join();
        s_run_thread = new std::thread(&ExampleClass::selection_run, this);
    }
    ImGui::End();
}

void ExampleClass::selection_run(){

    Window& window = Window::instance();

    window.unselect_all_elements();
    for(int i = 0; i < selection_level; i++){
        window.select_element(hand, (i) % 10000,selection_type);
    }
};

void ExampleClass::bounding_demonstration_ui() {
    ImGui::Begin("Custom UI");
    // Next Phase
    if (ImGui::Button("Next"))
    {
        m_phase++;
        Window::instance().end();
    }
    float pre_thickness = bounding_box_thickness;

    ImGui::SliderFloat("Thickness", &bounding_box_thickness, 0, 1);

    if(bounding_box_thickness != pre_thickness)
        bounding_run();


    ImGui::End();
}

void ExampleClass::bounding_run() {
    double max_x = 0;
    double min_x = 0;
    double max_y = 0;
    double min_y = 0;
    double max_z = 0;
    double min_z = 0;
    // Bounding Run Setup

    // OVM Setup
    OpenVolumeMesh::GeometricPolyhedralMeshV3f m_mesh;

    OpenVolumeMesh::IO::FileManager file_manager;
    file_manager.readFile("../res/sample_meshes/hand4234.1.ovm", m_mesh);

    // VOS Window
    Window& window = Window::instance();
    window.set_mesh(&m_mesh);

    window.remove_all_shapes();

    auto mesh_obj = Window::instance().get_mesh_obj(hand);

    for (OpenVolumeMesh::VertexIter v_it = m_mesh.vertices_begin();
         v_it != m_mesh.vertices_end(); ++v_it)
    {
        double x = m_mesh.vertex(*v_it)[0];
        double y = m_mesh.vertex(*v_it)[1];
        double z = m_mesh.vertex(*v_it)[2];
        // X
        if(x < min_x)
            min_x = x;
        else if(x > max_x)
            max_x = x;
        // Y
        if(y < min_y)
            min_y = y;
        else if(y > max_y)
            max_y = y;
        // Z
        if(z < min_z)
            min_z = z;
        else if(z > max_z)
            max_z = z;

    }

    // Create Boxes Shapes around the Mesh
    double x_length = max_x - min_x;
    double y_length = max_y - min_y;
    double z_length = max_z - min_z;

    x_length += bounding_box_thickness;
    y_length += bounding_box_thickness;
    z_length += bounding_box_thickness;

    // Y-Z Sides

    Box* box = new Box(x_length, bounding_box_thickness,bounding_box_thickness);
    box->set_position(0, max_y, max_z);
    window.add_shape(box);
    box = new Box(x_length, bounding_box_thickness,bounding_box_thickness);
    box->set_position(0, min_y, max_z);
    window.add_shape(box);

    box = new Box(x_length, bounding_box_thickness,bounding_box_thickness);
    box->set_position(0,min_y, min_z);
    window.add_shape(box);

    box = new Box(x_length, bounding_box_thickness,bounding_box_thickness);
    box->set_position(0, max_y, min_z);
    window.add_shape(box);

    // X-Z Sides

    box = new Box(bounding_box_thickness, y_length,bounding_box_thickness);
    box->set_position(max_x, 0, max_z);
    window.add_shape(box);

    box = new Box(bounding_box_thickness, y_length,bounding_box_thickness);
    box->set_position(min_x, 0, max_z);
    window.add_shape(box);

    box = new Box(bounding_box_thickness, y_length,bounding_box_thickness);
    box->set_position(min_x, 0, min_z);
    window.add_shape(box);

    box = new Box(bounding_box_thickness, y_length,bounding_box_thickness);
    box->set_position(max_x, 0, min_z);
    window.add_shape(box);


    // X-Y Sides

    box = new Box(bounding_box_thickness, bounding_box_thickness, z_length);
    box->set_position(max_x, max_y, 0);
    window.add_shape(box);

    box = new Box(bounding_box_thickness, bounding_box_thickness,z_length);
    box->set_position(min_x, max_y, 0);
    window.add_shape(box);

    box = new Box(bounding_box_thickness, bounding_box_thickness,z_length);
    box->set_position(min_x, min_y, 0);
    window.add_shape(box);

    box = new Box(bounding_box_thickness, bounding_box_thickness,z_length);
    box->set_position(max_x, min_y, 0);
    window.add_shape(box);
}