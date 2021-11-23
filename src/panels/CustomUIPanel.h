//
// Created by projektgruppe on 23.11.21.
//

#ifndef VOLUMESHOS_CUSTOMUIPANEL_H
#define VOLUMESHOS_CUSTOMUIPANEL_H

#include <functional>
#include "../ImguiRenderer.h"

namespace vOS
{
    class CustomUIPanel : public WindowPanel {

    public:
        ~CustomUIPanel(){};
        void show() override;
        void set_custom_callback(std::function<void()> vc) {m_custom_callback_function = vc;};

    private:
        static void empty_callback_function() {};
        std::function<void()> m_custom_callback_function = empty_callback_function;
    };
}


#endif //VOLUMESHOS_CUSTOMUIPANEL_H
