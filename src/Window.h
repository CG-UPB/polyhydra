#pragma once

#include <GLFW/glfw3.h>
#include "panels/MenuBar.h"
#include "algorithms/VosWindow.h"

#include <string>
#include <vector>

namespace vOS
{
    class WindowPanel
    {
    public:
        virtual void show() = 0;
        virtual ~WindowPanel() = default;
    };

    class Window
    {

    public:
        Window(int width, int height, std::string title, VosWindow* vos_pointer);
        ~Window();
        void show();
        MenuBar* get_menu_bar(){return m_menu_bar;}
    private:

        void initGLFW();
        void initImGui();
        void initImGuiStyle();
        void initPanels();
        void showDockSpace();

        MenuBar* m_menu_bar;
        VosWindow* m_vos_window;

        int m_width;
        int m_height;
        std::string m_title;
        std::string m_glslVersion;
        GLFWwindow* m_window;
        std::vector<WindowPanel*> m_panels;
    };
}