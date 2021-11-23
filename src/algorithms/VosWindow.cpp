//
// Created by jan on 15.10.21.
//

#include "VosWindow.h"
#include <memory>
#include <thread>
#include "memory"
#include "../input/Input.h"
#include <utility>

#include "../panels/LogWindow.h"
#include "../panels/PropertyView.h"
#include "../panels/MeshView.h"
#include "../ImguiRenderer.h"
#include "../panels/MenuBar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "../panels/CustomUIPanel.h"
#include "../rendering/shapes/Box.h"
#include "../rendering/passes/ShapePass.h"


namespace vOS
{

    VosWindow& VosWindow::instance()
    {
        static VosWindow inst;
        return inst;
    }

    VosWindow::VosWindow()
    {
        // Create Custom UI Panel Object
        m_custom_ui = new CustomUIPanel();
    }

    VosWindow::~VosWindow()
    {
    }

    void VosWindow::initPanels()
    {
        m_menu_bar = new MenuBar();
        m_panels.push_back(m_menu_bar);

        auto* mesh_view = new MeshView(720, 480);

        m_panels.push_back(mesh_view);
        m_panels.push_back(m_custom_ui);
        LogWindow* mylog = LogWindow::getInstance();
        m_panels.push_back(mylog);
    }

    void VosWindow::debugging_template_ui() {

        ImGui::Begin("Custom UI");
        // Pause Button
        if(m_pause_toggled)
        {
            // Pause button is active, pressing it would undo pause

            if(ImGui::Button(">"))
            {
                m_pause_toggled = false;
                VosWindow::instance().m_on_vos_unpaused();
            }
        }else{
            // Pause button is inactive, pressing it would pause

            if(ImGui::Button("||"))
            {
                m_pause_toggled = true;
                VosWindow::instance().m_on_vos_paused();
            }
        }
        // Reset Button
        if(ImGui::Button("Reset"))
        {
            VosWindow::instance().m_on_reset();
        }

        // Step Button
        if(ImGui::Button("Step"))
        {
            VosWindow::instance().m_on_step();
        }

        ImGui::End();
    }

    void VosWindow::open()
    {
        m_window_open = true;
        m_imgui_renderer = new ImguiRenderer(1280, 720, "volumeshOS");

        // Create default UI Panels
        initPanels();

        m_initialized = true;
    }

    void VosWindow::render_loop() {

        // Render window forever
        while(m_window_open)
        {
            render();
        }
    }

    bool VosWindow::render_manual() {

        // Don't render if the window is not running
        if(!is_running())
            return false;

        // Render single frame
        render();

        // Give feedback on success of rendered frame
        return m_window_open;
    }
    void VosWindow::close() {

        // Destroy Imgui Elements
        for (auto& element: m_panels) {
            if(element == LogWindow::getInstance())
                continue;
            delete element;
        }

        if(m_imgui_renderer != nullptr)
            delete m_imgui_renderer;
        m_window_open = false;
    }

    void VosWindow::render() {

        // Activate Mutex Guard

        // Query whether the window has been closed by the user or not
        bool window_closed = m_imgui_renderer->window_closed();

        if (window_closed) {
            std::cout << "Window Closed" << std::endl;
            m_window_open = false;
            return;
        }

        // Pre Render Setup
        get_mesh_obj().m_is_rendering = true;
        m_imgui_renderer->pre_render_step();

        // Draw all of our panels
        for (auto &element: m_panels) {
            element->show();
        }

        // Post Render Stuff
        m_imgui_renderer->post_render_step();
        get_mesh_obj().m_is_rendering = false;

        // Deactivate Mutex Guard
    }

    void VosWindow::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh)
    {
        m_mesh_obj.set_mesh(mesh);
    }

    MeshObject& VosWindow::get_mesh_obj()
    {
        return m_mesh_obj;
    }

    bool VosWindow::is_ready()
    {
        return m_initialized;
    }

    void VosWindow::set_vertex_color(OpenVolumeMesh::VertexHandle v_h, bool b, float red, float green, float blue, float alpha)
    {
        m_mesh_obj.set_highlight(v_h, b, red, green, blue, alpha);
    }

    bool VosWindow::is_running()
    {
        return m_window_open;
    }

    void VosWindow::set_callback_paused(void_callback vc)
    {
        m_on_vos_paused = vc;
    }

    void VosWindow::set_callback_unpaused(void_callback vc)
    {
        m_on_vos_unpaused = vc;
    }


    bool VosWindow::is_closed() {
        return is_running();
    }

    void VosWindow::default_callback_function()
    {
        LogWindow::getInstance()->addLog("Debug: Default Callback Function Called");
    }

    unsigned int VosWindow::add_box(float x, float y, float z, float red, float green, float blue)
    {
        while (m_mesh_obj.m_is_rendering)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        Box* box = new Box(0.05f, 0.05f, 0.05f);
        box->set_transform(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
        box->set_base_color(glm::vec4(red, green, blue, 1.0f));
        return ShapePass::add_shape(box);
    }
}