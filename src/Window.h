#pragma once

#include "vospch.h"

#include <GLFW/glfw3.h>
#include "panels/CustomUIPanel.h"

namespace volumeshOS::Internal
{
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

        virtual ~WindowPanel() = default;
    };

    /**
     * Responsible for Imgui and GL Setup and communication
     */
    class Window
    {

    public:
        /**
         * Handles and renders window docking function of ImGui
         */
        static void show_dock_space();

        static void load_dark_mode();
        static void load_light_mode();

    public:

        Window(int width, int height, std::string title);

        void initialize();
        void clean_up();

        /**
         * @return True if window is closed
         */
        bool should_close();

        void close();

        void render();

        /**
         * @return GLFW Window pointer
         */
        GLFWwindow* get_window() { return m_window; }

    private:

        static void init_style();

    private:

        void init_glfw();

        void init_imgui();

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

    private:

        struct
        {
            Internal::CustomUIPanel custom_ui;

        } m_panels;

        bool m_open = false;
        int m_width;
        int m_height;
        std::string m_title;
        std::string m_glslVersion;
        GLFWwindow* m_window;
    };
}