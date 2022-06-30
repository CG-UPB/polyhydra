#pragma once

#include "vospch.h"

#include <GLFW/glfw3.h>
#include "panels/LogWindow.h"
#include "panels/MeshLayerView.h"
#include "panels/MeshView.h"
#include "panels/QualityPanel.h"
#include "panels/ToolBar.h"

namespace volumeshOS::Internal
{
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

    public:

        struct
        {
            std::shared_ptr<LogWindow> log_window           = nullptr;
            std::shared_ptr<MeshLayerView> mesh_layer_view  = nullptr;
            std::shared_ptr<MeshView> mesh_view             = nullptr;
            std::shared_ptr<QualityPanel> quality_settings  = nullptr;
            std::shared_ptr<ToolBar> toolbar                = nullptr;
        } panels;

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

        bool m_open = false;
        int m_width;
        int m_height;
        std::string m_title;
        std::string m_glslVersion;
        GLFWwindow* m_window;
    };
}