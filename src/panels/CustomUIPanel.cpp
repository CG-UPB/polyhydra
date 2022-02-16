//
// Created by projektgruppe on 23.11.21.
//

#include "CustomUIPanel.h"

namespace vOS {

    void CustomUIPanel::show() {
        // if costum_callback_function(default) is empty nothing happens
        m_custom_callback_function();
    }
}