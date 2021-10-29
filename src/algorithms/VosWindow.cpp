//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <memory>
#include <thread>
#include "../Window.h"
#include "memory"

namespace vOS
{

    VosWindow& VosWindow::instance()
    {
        static VosWindow inst;
        return inst;
    }

    VosWindow::VosWindow()
    {
        // Start main loop thread
        VosWindow::instance().main_loop_thread = new std::thread(&VosWindow::main_loop, this);
    }


    VosWindow::~VosWindow()
    {
        VosWindow::instance().main_loop_thread->join();
    }

    void VosWindow::main_loop()
    {

        VosWindow::instance().m_running = true;
        VosWindow::instance().m_window = new Window(1280, 720, "volumeshOS");

        VosWindow::instance().m_initialized = true;

        // Render window
        VosWindow::instance().m_window->show();

        delete VosWindow::instance().m_window;
        VosWindow::instance().m_running = false;
    }

    void VosWindow::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh)
    {
        VosWindow::instance().m_mesh_obj.set_mesh(mesh);
    }

    MeshObject& VosWindow::get_mesh_obj()
    {
        return VosWindow::instance().m_mesh_obj;
    }

    void VosWindow::set_callback_pause_activated(void_callback vc)
    {
        VosWindow::on_pause_activated = vc;
    }

    void VosWindow::set_callback_pauseDeactivated(void_callback vc)
    {
        VosWindow::on_reset_pressed = vc;
    }

    void VosWindow::set_callback_on_reset(void_callback vc)
    {
        VosWindow::on_step_pressed = vc;
    }

    void VosWindow::set_callback_on_step(void_callback vc)
    {
        VosWindow::on_pause_activated = vc;
    }

    bool VosWindow::is_paused()
    {
        return false;
    }

    bool VosWindow::next_step_allowed()
    {
        return VosWindow::instance().m_initialized;
    }

    void VosWindow::set_vertex_color(OpenVolumeMesh::VertexHandle *vertices_array, float r, float g, float b, float a)
    {
        return;
    }

    bool VosWindow::is_running()
    {
        return VosWindow::instance().m_running;
    }
}