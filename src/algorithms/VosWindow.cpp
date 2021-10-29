//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <memory>
#include <thread>
#include "../Window.h"
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
        m_window = new Window(1280, 720, "volumeshOS");

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

    void VosWindow::SetCallbackPauseActivated(void_callback vc) {
        VosWindow::OnPauseActivated = vc;
    }

    void VosWindow::SetCallbackPauseDeactivated(void_callback vc) {
        VosWindow::OnResetPressed = vc;
    }
    void VosWindow::SetCallbackOnReset(void_callback vc) {
        VosWindow::OnStepPressed = vc;
    }

    void VosWindow::SetCallbackOnStep(void_callback vc) {
        VosWindow::OnPauseActivated = vc;
    }

    bool VosWindow::IsPaused() {
        return false;
    }

    bool VosWindow::NextStepAllowed() {
        return m_initialized;
    }

    void VosWindow::SetColor(OpenVolumeMesh::VertexHandle* vertices_array, float r, float g, float b, float a)
    {

    }

    bool VosWindow::is_running()
    {
        return m_running;
    }
}