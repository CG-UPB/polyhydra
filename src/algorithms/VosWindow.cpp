//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <thread>
#include "../Window.h"

namespace vOS{

    VosWindow::VosWindow(){
        // Setup
    }

    VosWindow::VosWindow(OpenVolumeMesh::GeometricPolyhedralMeshV3f* mesh){
        // Setup
        LinkMesh(mesh);
    }

    VosWindow::~VosWindow(){
        main_loop_thread->join();
    }

    void VosWindow::Main_Loop() {
        // Create window
        m_debug_window= new Window(1280, 720, "volumeshOS");
        // Render window
        m_debug_window->show();
    }

    std::thread::id VosWindow::Open() {

        // Make Pointer to the mesh to a shared pointer
        if(m_linear_mesh_pointer != nullptr)
            m_thread_mesh_pointer = std::make_shared<v3f>(*m_linear_mesh_pointer);

        // Start main loop thread
        main_loop_thread = new std::thread(&VosWindow::Main_Loop, this);

        // Return thread id
        return main_loop_thread->get_id();
    }

    bool VosWindow::LinkMesh(v3f *mesh) {
        m_linear_mesh_pointer = mesh;
        return true;
    }

    bool VosWindow::RemoveMesH(v3f *mesh) {
        m_linear_mesh_pointer = nullptr;
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
        return true;
    }
    bool VosWindow::IsOnCooldown() {
        return false;
    }
}