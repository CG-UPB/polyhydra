#pragma once

#include "../Window.h"
#include "../algorithms/VosWindow.h"
namespace vOS
{
    class MenuBar: public WindowPanel
    {
        void show() override;
        ~MenuBar() override = default;
    public:

        void set_vos_window(VosWindow* vos_pointer);
        bool pause_is_pressed();

        // A Reference pointer to our Vos Window
        VosWindow* m_vos_window;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///                                               Menu States
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool m_pause_toggled = false;

    };
}