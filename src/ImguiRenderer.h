#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include "rendering/gl/FrameBufferObject.h"

namespace vOS
{

    // Forward Declaration
    class Window;

    class WindowPanel
    {
    public:
        virtual void show() = 0;
        virtual ~WindowPanel() = default;
    };
    class MenuBar;

    class ImguiRenderer
    {

    public:

        ImguiRenderer(int width, int height, std::string title);
        ~ImguiRenderer();

        bool window_closed();
        void show_dock_space();
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