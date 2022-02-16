#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include "rendering/gl/FrameBufferObject.h"

namespace vOS
{

    // Forward Declaration
    class Window;

    /**
     * class WindowPanel which is used to initialise and show all of our panels
     */
    class WindowPanel
    {
    public:
        /**
         * show-method which is called in every time step. Each of the inherited classes has to implement the show method
         */
        virtual void show() = 0;
        virtual ~WindowPanel() = default;
    };
    class MenuBar;

    class ImguiRenderer
    {

    public:
        ImguiRenderer(int width, int height, std::string title);
        ~ImguiRenderer();
        void show_dock_space();
        bool window_closed();
        void pre_render_step();
        void post_render_step();
        GLFWwindow* get_window(){return m_window;}
    private:

        void initGLFW();
        void initImGui();
        void initImGuiStyle();

        int m_width;
        int m_height;
        std::string m_title;
        std::string m_glslVersion;
        GLFWwindow* m_window;
    };
}