
#include "CustomUIPanel.h"

namespace vOS {

    void CustomUIPanel::show() {
        // if custom_callback_function(default) is empty nothing happens
        m_custom_callback_function();
    }
}