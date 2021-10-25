//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <thread>
#include "../Window.h"

namespace vOS{

    VosWindow::VosWindow(){
        // Start main loop thread
        main_loop_thread = new std::thread(&VosWindow::Main_Loop, this);

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

    bool VosWindow::LinkMesh(v3f *mesh) {
        return true;
    }

    bool VosWindow::RemoveMesH(v3f *mesh) {
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
}