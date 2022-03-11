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
     *  Parent Class for any visible Windows inside the VosViewer such as the Logger, Toolbox and Mesh List
     */
    class WindowPanel
    {
    public:
        /**
         * Renders the Window Panel. Intended to be run each frame. Also responsible for Panel-internal logic
         */
        virtual void show() = 0;
        /**
         * Destructor
         */
        virtual ~WindowPanel() = default;
    };
    /**
     * Responsible for Imgui and GL Setup and communication
     */
    class ImguiRenderer
    {

    public:
        /**
         * Constructor
         * @param width
         * @param height
         * @param title
         */
        ImguiRenderer(int width, int height, std::string title);
        /**
         * Deconstructor
         */
        ~ImguiRenderer();
        /**
         * Handles and renders window docking function of ImGui
         */
        void show_dock_space();
        /**
         * @return True if window is closed
         */
        bool window_closed();
        /**
         * Imgui pre render frame setup
         * Must be called before any ImGui logic can be used via any panel
         * Renders Dock Space
         */
        void pre_render_step();
        /**
         * Imgui post render frame cleanup
         * Must be called last each frame, to properly end ImGui functionality
         * Renders UI
         */
        void post_render_step();

        void load_dark_mode();
        void load_light_mode();

        /**
         * @return GLFW Window pointer
         */
        GLFWwindow* get_window(){return m_window;}
    private:

        /**
         * Initializes GLFW
         * This operation is Thread local!
         */
        void initGLFW();
        /**
         * Initializes ImGui
         */
        void initImGui();
        /**
         * Initializes ImGui Style
         * The UI style choices are made here
         */
        void initImGuiStyle();

        /**
         * Window width
         */
        int m_width;
        /**
         * Window height
         */
        int m_height;
        /**
         * Window name
         */
        std::string m_title;
        /**
         * GLSL Version
         */
        std::string m_glslVersion;
        /**
         * GLFW Window
         */
        GLFWwindow* m_window;
    };
}