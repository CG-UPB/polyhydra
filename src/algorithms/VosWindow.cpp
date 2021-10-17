//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"

namespace vOS{

    VosWindow::VosWindow(){
        // Setup
    }

    VosWindow::~VosWindow(){

    }

    bool VosWindow::OpenWindow() {

        return true;
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