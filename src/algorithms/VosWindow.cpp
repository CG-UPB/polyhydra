//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <memory>
#include <thread>
#include "../Window.h"
#include "../panels/MenuBar.h"
#include "memory"

namespace vOS{

    VosWindow::VosWindow() {
        // Start main loop thread
        main_loop_thread = new std::thread(&VosWindow::Main_Loop, this);
    }

    VosWindow::VosWindow(OpenVolumeMesh::GeometricPolyhedralMeshV3f* mesh) {
        // Setup
        LinkMesh(mesh);
        main_loop_thread = new std::thread(&VosWindow::Main_Loop, this);
    }

    VosWindow::~VosWindow(){
        main_loop_thread->join();
    }

    void VosWindow::Main_Loop() {

        m_running = true;
        VosWindow* link_to_this_class = this;
        m_window = new Window(1280, 720, "volumeshOS");
        m_window->set_vos_window(link_to_this_class);

        m_initialized = true;

        // Render window
        m_window->show();

        delete m_window;
        m_running = false;
    }

    bool VosWindow::LinkMesh(v3f *mesh) {
        m_mesh_reference = mesh;
        return true;
    }

    bool VosWindow::RemoveMesH(v3f *mesh) {
        m_mesh_reference = nullptr;
        return true;
    }

    bool VosWindow::is_paused() {
        return m_window->get_menu_bar()->pause_is_pressed();
    }

    bool VosWindow::is_ready() {
        return m_initialized;
    }

    void VosWindow::SetColor(OpenVolumeMesh::VertexHandle* vertices_array, float r, float g, float b, float a)
    {

    }

    bool VosWindow::is_running()
    {
        return m_running;
    }

    void VosWindow::set_callback_paused(void_callback vc) {
        m_on_vos_paused = vc;
    }

    void VosWindow::set_callback_unpaused(void_callback vc) {
        m_on_vos_unpaused = vc;
    }

    void VosWindow::add_log(const char* fmt, int level)
    {
        Log()->addLog(fmt, level);
    }

    void VosWindow::default_callback_function() { LogWindow::getInstance()->addLog("Debug: Default Callback Function Called");}
}