//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <memory>
#include <thread>
#include "../Window.h"
#include "../panels/MenuBar.h"
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
        if (main_loop_thread  == nullptr)
        {
            main_loop_thread = new std::thread(&VosWindow::main_loop, this);
        }
    }

    VosWindow::~VosWindow()
    {
        main_loop_thread->join();
    }

    void VosWindow::main_loop()
    {

        m_running = true;
        m_window = new Window(1280, 720, "volumeshOS");

        m_initialized = true;

        // Render window
        m_window->show();

        delete m_window;
        m_running = false;
    }

    void VosWindow::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh)
    {
        m_mesh_obj.set_mesh(mesh);
    }

    MeshObject& VosWindow::get_mesh_obj()
    {
        return m_mesh_obj;
    }

    bool VosWindow::is_paused()
    {
        return m_window->get_menu_bar()->pause_is_pressed();
    }

    bool VosWindow::is_ready()
    {
        return m_initialized;
    }

    void VosWindow::set_vertex_color(OpenVolumeMesh::VertexHandle *vertices_array, float r, float g, float b, float a)
    {
        return;
    }

    bool VosWindow::is_running()
    {
        return VosWindow::instance().m_running;
    }

    void VosWindow::set_callback_paused(void_callback vc)
    {
        m_on_vos_paused = vc;
    }

    void VosWindow::set_callback_unpaused(void_callback vc)
    {
        m_on_vos_unpaused = vc;
    }

    void VosWindow::add_log(const char* fmt, int level)
    {
        Log()->addLog(fmt, level);
    }

    void VosWindow::default_callback_function()
    {
        LogWindow::getInstance()->addLog("Debug: Default Callback Function Called");
    }
}