//
// Created by projektgruppe on 23.11.21.
//

#ifndef VOLUMESHOS_CUSTOMUIPANEL_H
#define VOLUMESHOS_CUSTOMUIPANEL_H

#include <functional>
#include "../ImguiRenderer.h"

namespace vOS
{
    /**
     * This class represents our CustomUIPanel. The User is able to create its own UI with Imgui-Elements. It could be
     * used with our filedialog to get meshes inside vOS, or to get some values(ints, floats, etc.) which are relevant for
     * algorithms. For more information's watch our examples
     */
    class CustomUIPanel : public WindowPanel {

    public:
        ~CustomUIPanel(){};
        /**
         *  This method is used to draw the CustomUI in every loop. Therefore it only calls the costum_callback funtion
         */
        void show() override;

        /**
         * this method can assign a callback function to the Costum-UI. The callback-function has to work with Imgui,
         * else there will appear big errors
         *
         * @param vc the callback-function for the UI
         */
        void set_custom_callback(std::function<void()> vc) {m_custom_callback_function = vc;};

    private:

        static void empty_callback_function() {};
        std::function<void()> m_custom_callback_function = empty_callback_function;
    };
}


#endif //VOLUMESHOS_CUSTOMUIPANEL_H
