//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <thread>

namespace vOS{

    VosWindow::VosWindow(){
        // Setup
    }

    VosWindow::~VosWindow(){

    }

    void VosWindow::Main_Loop() {

    }

    int VosWindow::OpenWindow() {
        //std::thread newie(VosWindow::Main_Loop);

        //main_loop_thread = &newie;
        return 0;
    }

    bool VosWindow::LinkMesh(v3f *mesh) {
        m_mesh = mesh;
        return true;
    }

    bool VosWindow::RemoveMesH(v3f *mesh) {
        m_mesh = nullptr;
        return true;
    }

    void VosWindow::SetCallbackPauseActivated(void_callback vc) {
        VosWindow::OnPauseActivated = vc;
    }

    void VosWindow::SetCallbackPauseDeactivated(void_callback vc) {
        VosWindow::OnPauseDeactivated = vc;

    }

    bool VosWindow::VosPauseActive() {
        return false;
    }

    bool VosWindow::VosSideReady() {
        return true;
    }
}