//
// Created by jan on 15.10.21.
//

#include "Window.h"
#include <memory>
#include <thread>
#include "memory"
#include "input/Input.h"
#include <utility>
#include <OpenVolumeMesh/FileManager/FileManager.hh>

#include "panels/LogWindow.h"
#include "panels/MeshView.h"
#include "ImguiRenderer.h"
#include "panels/MenuBar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "panels/CustomUIPanel.h"
#include "rendering/passes/ShapePass.h"


namespace vOS
{
    FileDialog *Window::m_file_dialog;

    Window &Window::instance()
    {
        static Window inst;
        return inst;
    }

    Window::Window()
    {
        // Create Custom UI Panel Object
        m_custom_ui = new CustomUIPanel();
    }

    Window::~Window()
    {
    }

    void Window::initPanels()
    {
        m_file_dialog = new FileDialog();
        m_menu_bar = new MenuBar();
        m_panels.push_back(m_menu_bar);

        auto *mesh_view = new MeshView(720, 480);

        m_panels.push_back(mesh_view);
        m_panels.push_back(m_file_dialog);
        m_panels.push_back(m_custom_ui);
        LogWindow *mylog = LogWindow::getInstance();
        m_panels.push_back(mylog);
    }

    void Window::open()
    {
        m_window_open = true;
        m_imgui_renderer = new ImguiRenderer(1280, 720, "volumeshOS");

        // Create default UI Panels
        initPanels();

        m_initialized = true;
    }

    void Window::run(void_callback vc)
    {
        set_custom_imgui(vc);

        open();

        // Render window forever until window is closed by user
        while (m_window_open)
        {
            // Render single frame
            render();
        }

        close();
    }

    void Window::run()
    {

        open();

        // Render window forever until window is closed by user
        while (m_window_open)
        {
            // Render single frame
            render();
        }

        close();
    }

    void Window::close()
    {

        // Destroy Imgui Elements
        for (auto &element: m_panels)
        {
            if (element == LogWindow::getInstance())
                continue;
            delete element;
        }

        if (m_imgui_renderer != nullptr)
            delete m_imgui_renderer;
        m_window_open = false;
    }

    void Window::render()
    {

        // Activate Mutex Guard

        // Query whether the window has been closed by the user or not
        bool window_closed = m_imgui_renderer->window_closed();

        if (window_closed)
        {
            std::cout << "Window Closed" << std::endl;
            m_window_open = false;
            return;
        }

        // Pre Render Setup
        get_mesh_obj().m_is_rendering = true;
        m_imgui_renderer->pre_render_step();

        // Draw all of our panels
        for (auto &element: m_panels)
        {
            element->show();
        }

        // Post Render Stuff
        m_imgui_renderer->post_render_step();
        get_mesh_obj().m_is_rendering = false;

        // Deactivate Mutex Guard
    }

    void Window::set_mesh(OpenVolumeMesh::GeometryKernel<OpenVolumeMesh::Vec3f> *mesh, int index)
    {
        auto mesh_obj = new MeshObject();
        mesh_obj->set_mesh(mesh);

        bool replaced = false;
        // check if index of mesh already exist: yes -> replace it, no -> just insert it
        for(auto & m: m_mesh_objects)
        {
            if (m.first == index)
            {
                m.second = mesh_obj;
                replaced = true;
            }
        }
        if (!replaced)
        {
            m_mesh_objects.insert({index, mesh_obj});
        }
    }

    MeshObject &Window::get_mesh_obj(int index)
    {
        for (auto &m: m_mesh_objects)
        {
            if (m.first == index)
            {
                return *m.second;
            }
        }
        return m_mesh_obj;
    }

    bool Window::is_ready()
    {
        return m_initialized;
    }

    void
    Window::set_vertex_color(OpenVolumeMesh::VertexHandle v_h, bool b, float red, float green, float blue, float alpha)
    {
        m_mesh_obj.set_highlight(v_h, b, red, green, blue, alpha);
    }

    bool Window::is_running()
    {
        return m_window_open;
    }


    bool Window::is_closed()
    {
        return is_running();
    }

    void Window::default_callback_function()
    {
        LogWindow::getInstance()->addLog("Debug: Default Callback Function Called");
    }

    unsigned int Window::add_shape(Shape *shape)
    {
        // TODO Add good exception handling
        if (shape == nullptr)
            return -1;

        return ShapePass::add_shape(shape);
    }

    /*
    unsigned int Window::add_box(float x, float y, float z, float red, float green, float blue)
    {
        Box* box = new Box(0.05f, 0.05f, 0.05f);
        box->set_transform(glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
        box->set_base_color(glm::vec4(red, green, blue, 1.0f));
        return ShapePass::add_shape(box);
    }*/

    bool Window::ShowFileDialog(std::string &path, const std::string &extension)
    {
        m_file_dialog->open(extension);
        if (m_file_dialog->is_ok())
        {
            path = m_file_dialog->get_file_path();
            m_file_dialog->set_open(false);
        }
        return m_file_dialog->is_ok();
    }
}