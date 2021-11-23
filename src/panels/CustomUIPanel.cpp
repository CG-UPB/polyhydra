//
// Created by projektgruppe on 23.11.21.
//

#include "CustomUIPanel.h"
#include "imgui.h"

namespace vOS {

    void CustomUIPanel::show() {
        ImGui::Begin("Custom UI");
        m_custom_callback_function();
        ImGui::End();
    }
}