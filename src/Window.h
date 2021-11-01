#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include "rendering/FrameBufferObject.h"

namespace vOS
{
    class WindowPanel
    {
    public:
        virtual void show() = 0;
        virtual ~WindowPanel() = default;
    };

    // Forward Declaration
    class VosWindow;
    class MenuBar;

    class Window
    {

    public:
        Window(int width, int height, std::string title);
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

        int m_width;
        int m_height;
        std::string m_title;
        std::string m_glslVersion;
        GLFWwindow* m_window;
        std::vector<WindowPanel*> m_panels;
        FrameBufferObject* m_viewport_framebuffer;
    };
}